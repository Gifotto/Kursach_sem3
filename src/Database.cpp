#include "Database.h"

using namespace std;

Database::~Database() { if (conn) PQfinish(conn); }

bool Database::connect() {
    if (conn) PQfinish(conn);
    // Параметры совпадают с docker-compose.yml
    conn = PQconnectdb("host=localhost port=5432 dbname=sudoku user=sudoku password=sudoku");
    bool ok = PQstatus(conn) == CONNECTION_OK;
    if (!ok) lastError_r = PQerrorMessage(conn);
    return ok;
}

bool Database::ensureConnection() {
    return (conn && PQstatus(conn) == CONNECTION_OK) || connect();
}

bool Database::query(const string& sql, const vector<string>& params, Rows* out) {
    if (!ensureConnection()) return false;
    vector<const char*> values;
    for (auto& p : params) values.push_back(p.c_str());
    PGresult* r = PQexecParams(conn, sql.c_str(), (int)params.size(), nullptr,
                               values.data(), nullptr, nullptr, 0);
    auto st = PQresultStatus(r);
    bool ok = st == PGRES_COMMAND_OK || st == PGRES_TUPLES_OK;
    if (!ok) lastError_r = PQresultErrorMessage(r);
    if (ok && out) {
        out->clear();
        for (int i = 0; i < PQntuples(r); i++) {
            vector<string> row;
            for (int j = 0; j < PQnfields(r); j++) row.push_back(PQgetvalue(r, i, j));
            out->push_back(row);
        }
    }
    PQclear(r);
    return ok;
}

LoginInfo Database::login(const string& nick, const string& pass) {
    LoginInfo info;
    Rows r;
    // crypt(пароль, хэш) пересчитывает хэш с той же солью и сравнивает
    if (!query("SELECT u.id, r.name, u.nickname FROM users u JOIN roles r ON r.id = u.role_id "
               "WHERE lower(u.nickname)=lower($1) AND u.password_hash = crypt($2, u.password_hash)",
               {nick, pass}, &r)) {
        info.dbError = true;
        return info;
    }
    if (!r.empty()) {
        info.ok = true;
        info.id = stoi(r[0][0]);
        info.isAdmin = r[0][1] == "admin";
        info.nickname = r[0][2];
        query("INSERT INTO login_history(user_id) VALUES($1)", {r[0][0]});  // журнал входов
    }
    return info;
}

RegisterResult Database::registerUser(const string& nick, const string& pass) {
    Rows r;
    if (!query("SELECT 1 FROM users WHERE lower(nickname)=lower($1)", {nick}, &r)) return RegisterResult::Error;
    if (!r.empty()) return RegisterResult::Taken;
    // gen_salt('bf') — случайная соль bcrypt; пароль в открытом виде не хранится
    bool ok = query("INSERT INTO users(nickname, password_hash, role_id) VALUES($1, crypt($2, gen_salt('bf')), "
                    "(SELECT id FROM roles WHERE name='user'))", {nick, pass});
    return ok ? RegisterResult::Ok : RegisterResult::Error;
}

Rows Database::topScores() {
    Rows r;
    // Для каждого игрока берём лучшую партию, затем сортируем и оставляем 5
    query("SELECT u.nickname, b.score, b.time_seconds FROM "
          "(SELECT DISTINCT ON (user_id) user_id, score, time_seconds FROM games "
          " ORDER BY user_id, score DESC, time_seconds) b "
          "JOIN users u ON u.id = b.user_id JOIN roles r ON r.id = u.role_id WHERE r.name <> 'admin' "
          "ORDER BY b.score DESC, b.time_seconds LIMIT 5", {}, &r);
    return r;
}

Rows Database::userRecords(int userId) {
    Rows r;
    query("SELECT g.score, g.time_seconds, to_char(g.played_at,'YYYY-MM-DD HH24:MI'), d.name "
          "FROM games g JOIN difficulties d ON d.id = g.difficulty_id "
          "WHERE g.user_id=$1 ORDER BY g.score DESC, g.time_seconds LIMIT 15", {to_string(userId)}, &r);
    return r;
}

void Database::addGame(int userId, int difficultyId, int score, int seconds) {
    query("INSERT INTO games(user_id, difficulty_id, score, time_seconds) VALUES($1,$2,$3,$4)",
          {to_string(userId), to_string(difficultyId), to_string(score), to_string(seconds)});
}

vector<Difficulty> Database::difficulties() {
    Rows r;
    vector<Difficulty> out;
    query("SELECT id, name, holes, score_multiplier FROM difficulties ORDER BY id", {}, &r);
    for (auto& row : r) out.push_back({stoi(row[0]), row[1], stoi(row[2]), stoi(row[3])});
    return out;
}

Rows Database::adminUsers() {
    Rows r;
    query("SELECT u.id, u.nickname, COALESCE(MAX(g.score),0), COUNT(g.id) FROM users u "
          "JOIN roles r ON r.id = u.role_id LEFT JOIN games g ON g.user_id=u.id WHERE r.name <> 'admin' "
          "GROUP BY u.id ORDER BY lower(u.nickname)", {}, &r);
    return r;
}

Rows Database::adminGames(int userId) {
    Rows r;
    query("SELECT id, score, time_seconds FROM games WHERE user_id=$1 ORDER BY id", {to_string(userId)}, &r);
    return r;
}

Rows Database::loginHistory(int userId) {
    Rows r;
    query("SELECT id, to_char(logged_at,'YYYY-MM-DD HH24:MI:SS') FROM login_history "
          "WHERE user_id=$1 ORDER BY logged_at DESC, id DESC LIMIT 200", {to_string(userId)}, &r);
    return r;
}

bool Database::updateNick(int userId, const string& nick) {
    return query("UPDATE users SET nickname=$1 WHERE id=$2", {nick, to_string(userId)});
}
bool Database::setPassword(int userId, const string& pass) {
    // новый хэш с новой солью; менять можно только пароли обычных пользователей
    return query("UPDATE users SET password_hash = crypt($1, gen_salt('bf')) "
                 "WHERE id=$2 AND role_id = (SELECT id FROM roles WHERE name='user')", {pass, to_string(userId)});
}

void Database::updateScore(int gameId, int score) {
    query("UPDATE games SET score=$1 WHERE id=$2", {to_string(score), to_string(gameId)});
}
void Database::updateTime(int gameId, int seconds) {
    query("UPDATE games SET time_seconds=$1 WHERE id=$2", {to_string(seconds), to_string(gameId)});
}
void Database::deleteGame(int gameId) { query("DELETE FROM games WHERE id=$1", {to_string(gameId)}); }
void Database::deleteUser(int userId) {
    // админа удалить нельзя: удаляем только пользователей с ролью user
    query("DELETE FROM users WHERE id=$1 AND role_id = (SELECT id FROM roles WHERE name='user')", {to_string(userId)});
}