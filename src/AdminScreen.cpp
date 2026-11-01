#include "AdminScreen.h"
#include <algorithm>
#include "Validator.h"

static const int kVisibleRows = 12;  // строк в таблице без прокрутки

void AdminScreen::onEnter() {
    selUser = selGame = scrollUsers = scrollGames = 0;
    showLog = false;
    valueA.text.clear();
    valueB.text.clear();
    levels = app.db.difficulties();
    levelIdx = 0;
    reload();
}

void AdminScreen::reload() {
    users = app.db.adminUsers();
    // если выбранный пользователь исчез (удалён) — сбрасываем выбор
    bool found = std::any_of(users.begin(), users.end(), [&](auto& r) { return std::stoi(r[0]) == selUser; });
    if (!found) selUser = 0;
    loadDetails();
    found = std::any_of(games.begin(), games.end(), [&](auto& r) { return std::stoi(r[0]) == selGame; });
    if (!found) selGame = 0;
}

void AdminScreen::loadDetails() {
    games = selUser ? app.db.adminGames(selUser) : Rows{};
    loginRows = selUser ? app.db.loginHistory(selUser) : Rows{};
}

void AdminScreen::drawLists() {
    Ui& ui = app.ui;
    ui.text(L"Пользователи (ник / лучший счёт / игр)", 20, 45, 16, kGray);
    if (showLog) ui.text(sf::String(L"Журнал входов выбранного (последние ") + std::to_string(loginRows.size()) + L", UTC)", 460, 45, 16, kGray);
    else ui.text(L"Рекорды выбранного (id / очки / время)", 460, 45, 16, kGray);

    // прокрутка колесом мыши: левая половина окна — пользователи, правая — рекорды
    if (ui.wheel != 0) (ui.wheelX < 450 ? scrollUsers : scrollGames) -= (int)ui.wheel;
    scrollUsers = std::max(0, std::min(scrollUsers, (int)users.size() - kVisibleRows));
    scrollGames = std::max(0, std::min(scrollGames, (int)(showLog ? loginRows.size() : games.size()) - kVisibleRows));

    for (int i = 0; i < kVisibleRows; i++) {
        size_t k = i + scrollUsers;
        if (k >= users.size()) break;
        float y = 70 + i * 30;
        int id = std::stoi(users[k][0]);
        if (ui.rowBox(20, y, 420, id == selUser)) {  // выбор пользователя
            selUser = id; selGame = 0; scrollGames = 0;
            loadDetails();
        }
        ui.text(users[k][1], 26, y + 2, 18);
        ui.text(users[k][2], 270, y + 2, 18);
        ui.text(users[k][3], 360, y + 2, 18);
    }
    if (showLog) {
        // журнал входов: номер (1 - самый свежий вход) и дата-время
        for (int i = 0; i < kVisibleRows; i++) {
            size_t k = i + scrollGames;
            if (k >= loginRows.size()) break;
            float y = 70 + i * 30;
            ui.rowBox(460, y, 420, false);  // только фон строки
            ui.text(std::to_string(k + 1), 466, y + 2, 18);
            ui.text(loginRows[k][1], 540, y + 2, 18);
        }
        return;
    }
    for (int i = 0; i < kVisibleRows; i++) {
        size_t k = i + scrollGames;
        if (k >= games.size()) break;
        float y = 70 + i * 30;
        int id = std::stoi(games[k][0]);
        if (ui.rowBox(460, y, 420, id == selGame)) selGame = id;  // выбор рекорда
        ui.text(games[k][0], 466, y + 2, 18);
        ui.text(games[k][1], 580, y + 2, 18);
        ui.text(formatTime(std::stoi(games[k][2])) + " (" + games[k][2] + " s)", 700, y + 2, 18);
    }
}

void AdminScreen::changeNick() {
    std::wstring err = Validator::checkNick(valueA.text);
    if (!selUser) app.setMessage(L"Выберите пользователя");
    else if (!err.empty()) app.setMessage(err);
    else if (app.db.updateNick(selUser, valueA.text)) { app.setMessage(L"Ник изменён", false); reload(); }
    else app.setMessage(L"Такой ник уже занят");
}

void AdminScreen::changePassword() {
    // новый пароль берём из поля A; требования те же, что при регистрации
    std::wstring err = Validator::checkPassword(valueA.text, valueA.text);
    if (!selUser) app.setMessage(L"Выберите пользователя");
    else if (!err.empty()) app.setMessage(err);
    else if (app.db.setPassword(selUser, valueA.text)) app.setMessage(L"Пароль изменён", false);
    else app.setMessage(L"Не удалось изменить пароль");
}

void AdminScreen::changeScore() {
    if (showLog) { app.setMessage(L"Сначала переключитесь на рекорды"); return; }
    int v = 0;
    if (!selGame) app.setMessage(L"Выберите рекорд");
    else if (!Validator::parseInt(valueA.text, v)) app.setMessage(L"Введите целое число ≥ 0");
    else { app.db.updateScore(selGame, v); app.setMessage(L"Очки изменены", false); reload(); }
}

void AdminScreen::changeTime() {
    if (showLog) { app.setMessage(L"Сначала переключитесь на рекорды"); return; }
    int v = 0;
    if (!selGame) app.setMessage(L"Выберите рекорд");
    else if (!Validator::parseInt(valueA.text, v)) app.setMessage(L"Введите целое число секунд ≥ 0");
    else { app.db.updateTime(selGame, v); app.setMessage(L"Время изменено", false); reload(); }
}

void AdminScreen::createUser() {
    // A — ник, B — пароль; проверки такие же, как при самостоятельной регистрации
    std::wstring err = Validator::checkNick(valueA.text);
    if (err.empty()) err = Validator::checkPassword(valueB.text, valueB.text);
    if (!err.empty()) { app.setMessage(err); return; }

    switch (app.db.registerUser(valueA.text, valueB.text)) {
    case RegisterResult::Taken: app.setMessage(L"Такой ник уже занят"); return;
    case RegisterResult::Error: app.setMessage(L"Не удалось создать пользователя"); return;
    case RegisterResult::Ok: break;
    }
    app.setMessage(L"Пользователь создан", false);
    reload();
    // выбираем нового пользователя и прокручиваем список к нему
    for (size_t i = 0; i < users.size(); i++)
        if (users[i][1] == valueA.text) {
            selUser = std::stoi(users[i][0]);
            selGame = 0;
            loadDetails();
            if ((int)i >= scrollUsers + kVisibleRows) scrollUsers = (int)i - kVisibleRows + 1;
        }
}

void AdminScreen::addGame() {
    // A — очки, B — время в секундах, сложность — по кнопке; партия добавляется выбранному пользователю
    int score = 0, seconds = 0;
    if (!selUser) app.setMessage(L"Выберите пользователя");
    else if (levels.empty()) app.setMessage(L"В БД нет уровней сложности");
    else if (!Validator::parseInt(valueA.text, score) || !Validator::parseInt(valueB.text, seconds))
        app.setMessage(L"A — очки, B — секунды: целые числа ≥ 0");
    else {
        app.db.addGame(selUser, levels[levelIdx].id, score, seconds);
        app.setMessage(L"Партия добавлена", false);
        reload();
    }
}

void AdminScreen::drawControls() {
    Ui& ui = app.ui;
    ui.text(L"Поле A:", 20, 440, 16, kGray);
    ui.text(L"Поле B:", 230, 440, 16, kGray);
    ui.field(valueA, 20, 462, 200);
    ui.field(valueB, 230, 462, 200);
    // кнопка переключает сложность для добавляемых партий
    sf::String level = levels.empty() ? sf::String(L"-") : difficultyLabel(levels[levelIdx].name);
    if (ui.button(sf::String(L"Сложность: ") + level, 440, 462, 230, 32) && !levels.empty())
        levelIdx = (levelIdx + 1) % levels.size();

    // правка выбранного пользователя / рекорда (значение берётся из поля A)
    if (ui.button(L"Сменить ник", 20, 510, 145, 34)) changeNick();
    if (ui.button(L"Сменить пароль", 175, 510, 175, 34)) changePassword();
    if (ui.button(L"Очки", 360, 510, 90, 34)) changeScore();
    if (ui.button(L"Время (сек)", 460, 510, 130, 34)) changeTime();

    // создание
    if (ui.button(L"Создать пользователя", 20, 556, 240, 34)) createUser();
    if (ui.button(L"Добавить игру", 270, 556, 170, 34)) addGame();
    ui.text(L"Создать: A = ник, B = пароль.\nДобавить игру: A = очки, B = секунды\n(выбранному пользователю).", 450, 548, 14, kGray);

    // удаление и выход
    if (ui.button(L"Удалить рекорд", 20, 602, 180, 34)) {
        if (showLog) app.setMessage(L"Сначала переключитесь на рекорды");
        else if (!selGame) app.setMessage(L"Выберите рекорд");
        else { app.db.deleteGame(selGame); app.setMessage(L"Рекорд удалён", false); reload(); }
    }
    if (ui.button(L"Удалить пользователя", 210, 602, 245, 34)) {
        if (!selUser) app.setMessage(L"Выберите пользователя");
        else { app.db.deleteUser(selUser); app.setMessage(L"Пользователь удалён", false); reload(); }
    }
    // переключатель правой таблицы: рекорды <-> журнал входов выбранного пользователя
    if (ui.button(showLog ? L"Показать рекорды" : L"Журнал входов", 465, 602, 215, 34)) {
        showLog = !showLog;
        scrollGames = 0;
    }
    if (ui.button(L"Выйти из аккаунта", 690, 602, 200, 34)) app.logout();
    app.drawMessage(20, 654);
}

void AdminScreen::draw() {
    app.ui.text(L"Администратор", 20, 10, 28);
    drawLists();
    drawControls();
}