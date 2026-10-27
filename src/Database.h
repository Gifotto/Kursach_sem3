#pragma once
#include <libpq-fe.h>
#include <string>
#include <vector>

using Rows = std::vector<std::vector<std::string>>;  // результат запроса: строки × столбцы (текст)

// Результат попытки входа.
struct LoginInfo {
    bool ok = false;       // ник и пароль верны
    bool dbError = false;  // ошибка соединения/запроса
    bool isAdmin = false;
    int id = 0;
    std::string nickname;  // ник в том регистре, в каком он хранится в БД
};

// Уровень сложности из таблицы difficulties.
struct Difficulty {
    int id = 1;
    std::string name = "easy";  // easy / medium / hard
    int holes = 35;             // сколько клеток убрать
    int multiplier = 1;         // множитель очков
};

enum class RegisterResult { Ok, Taken, Error };

// Вся работа с PostgreSQL: подключение и готовые запросы приложения.
class Database {
public:
    ~Database();
    bool connect();
    const std::string& lastError() const { return lastError_r; }  // текст последней ошибки PostgreSQL

    // --- аккаунты ---
    LoginInfo login(const std::string& nick, const std::string& pass);
    RegisterResult registerUser(const std::string& nick, const std::string& pass);

    // --- рекорды ---
    Rows topScores();                                  // [ник, очки, время] — лучшие 5
    Rows userRecords(int userId);                      // [очки, время, дата, сложность] — до 15 лучших игр
    void addGame(int userId, int difficultyId, int score, int seconds);  // сохранить сыгранную партию
    std::vector<Difficulty> difficulties();            // справочник уровней сложности

    // --- администратор ---
    Rows adminUsers();                // [id, ник, лучший счёт, число игр]
    Rows adminGames(int userId);      // [id, очки, время]
    bool updateNick(int userId, const std::string& nick);  // false, если ник занят
    bool setPassword(int userId, const std::string& pass);  // новый пароль пользователя
    void updateScore(int gameId, int score);
    void updateTime(int gameId, int seconds);
    void deleteGame(int gameId);
    void deleteUser(int userId);      // вместе со всеми его рекордами (ON DELETE CASCADE)

private:
    // Выполняет параметризованный запрос (защита от SQL-инъекций). true при успехе.
    bool query(const std::string& sql, const std::vector<std::string>& params = {}, Rows* out = nullptr);
    bool ensureConnection();  // при обрыве/неудачном старте пробует подключиться заново
    PGconn* conn = nullptr;
    std::string lastError_r;
};
