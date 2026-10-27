#include "MenuScreen.h"

void MenuScreen::onEnter() {
    top = app.db.topScores();
    levels = app.db.difficulties();
    // если выбранного уровня нет в списке (первый заход) — берём первый
    bool found = false;
    for (auto& d : levels) if (d.id == app.difficulty.id) found = true;
    if (!found && !levels.empty()) app.difficulty = levels[0];
}

void MenuScreen::draw() {
    Ui& ui = app.ui;
    ui.text(sf::String(L"Игрок: ") + app.userName, 40, 30, 30);
    if (ui.button(L"Начать игру", 40, 120, 280, 50)) app.go(ScreenId::Game);
    if (ui.button(L"Мои рекорды", 40, 190, 280, 50)) app.go(ScreenId::Records);
    if (ui.button(L"Выйти из аккаунта", 40, 260, 280, 50)) app.logout();
    if (ui.button(L"Выйти из игры", 40, 330, 280, 50)) app.quit();

    // выбор сложности: выбранный уровень обведён синей рамкой
    ui.text(L"Сложность:", 40, 410, 20);
    for (size_t i = 0; i < levels.size(); i++) {
        float x = 40 + i * 108;
        if (levels[i].id == app.difficulty.id) { // эта кнопка сейчас выбрана?
            sf::RectangleShape frame({106, 40});
            frame.setPosition(x - 3, 437);
            frame.setFillColor(kBlue);
            ui.window.draw(frame);
        }
        if (ui.button(difficultyLabel(levels[i].name), x, 440, 100, 34)) app.difficulty = levels[i];
    }

    ui.text(L"Таблица рекордов (топ-5)", 400, 110, 28);
    ui.text(L"#   Ник", 400, 160, 20, kGray);
    ui.text(L"Очки", 650, 160, 20, kGray);
    ui.text(L"Время", 750, 160, 20, kGray);
    for (size_t i = 0; i < top.size(); i++) {
        float y = 195 + i * 36;
        ui.text(std::to_string(i + 1) + ".  " + top[i][0], 400, y);
        ui.text(top[i][1], 650, y);
        ui.text(formatTime(std::stoi(top[i][2])), 750, y);
    }
    if (top.empty()) ui.text(L"Пока нет рекордов", 400, 200, 20, kGray);
}
