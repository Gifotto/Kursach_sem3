#include "GameScreen.h"
#include <algorithm>

void GameScreen::newGame() {
    board.generate(app.difficulty.holes);  // число пустых клеток зависит от сложности
    selRow = selCol = -1;
    solved = false;
    hint = "";
    timer.restart();
}

void GameScreen::onText(uint32_t c) {
    if (solved || selRow < 0) return;
    if (c >= '1' && c <= '9') board.setValue(selRow, selCol, c - '0');
    else if (c == 8 || c == '0' || c == 127) board.setValue(selRow, selCol, 0);  // стереть
}

void GameScreen::checkAnswer() {
    switch (board.check()) {
    case SudokuBoard::CheckResult::Errors:
        hint = L"Есть ошибки\n (выделены красным)"; hintColor = kRed; break;
    case SudokuBoard::CheckResult::Incomplete:
        hint = L"Ошибок нет,\n но не все\n клетки заполнены"; hintColor = kGreen; break;
    case SudokuBoard::CheckResult::Solved:
        solved = true;
        finalSeconds = (int)timer.getElapsedTime().asSeconds();
        finalScore = std::max(100, 5000 - finalSeconds * 5) * app.difficulty.multiplier;  // быстрее и сложнее = больше очков
        app.db.addGame(app.userId, app.difficulty.id, finalScore, finalSeconds);
        break;
    }
}

void GameScreen::handleGridClick() {
    Ui& ui = app.ui;
    float mx = ui.mouse.x - kGridX, my = ui.mouse.y - kGridY;
    if (ui.clicked && !solved && mx >= 0 && my >= 0 && mx < 9 * kCell && my < 9 * kCell) {
        selCol = (int)(mx / kCell);
        selRow = (int)(my / kCell);
        ui.clicked = false;
    }
}

void GameScreen::drawGrid() {
    sf::RenderWindow& w = app.ui.window;
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++) {
            // фон клетки: выбранная — жёлтая, ошибочная — розовая
            sf::RectangleShape cell({kCell, kCell});
            cell.setPosition(kGridX + j * kCell, kGridY + i * kCell);
            bool sel = i == selRow && j == selCol;
            cell.setFillColor(sel ? sf::Color(255, 245, 160)
                                  : (board.isWrong(i, j) ? sf::Color(255, 220, 220) : sf::Color::White));
            w.draw(cell);
            int v = board.value(i, j);
            if (!v) continue;
            // цвет цифры: ошибка — красный, исходная — чёрный, введённая — синий
            sf::Text t(std::to_string(v), app.ui.font, 36);
            t.setFillColor(board.isWrong(i, j) ? kRed : (board.isGiven(i, j) ? sf::Color::Black : kBlue));
            auto bb = t.getLocalBounds();
            t.setPosition(kGridX + j * kCell + (kCell - bb.width) / 2 - bb.left,
                          kGridY + i * kCell + (kCell - bb.height) / 2 - bb.top);
            w.draw(t);
        }
    // линии сетки: каждая третья — толстая (границы блоков 3×3)
    for (int k = 0; k <= 9; k++) {
        float th = k % 3 == 0 ? 4 : 1;
        sf::RectangleShape h({9 * kCell + th, th}), v({th, 9 * kCell + th});
        h.setFillColor(sf::Color::Black);
        v.setFillColor(sf::Color::Black);
        h.setPosition(kGridX - th / 2, kGridY + k * kCell - th / 2);
        v.setPosition(kGridX + k * kCell - th / 2, kGridY - th / 2);
        w.draw(h);
        w.draw(v);
    }
}

void GameScreen::drawSidePanel() {
    Ui& ui = app.ui;
    ui.text(sf::String(L"Сложность: ") + difficultyLabel(app.difficulty.name), 620, 130, 22);
    if (ui.button(L"Проверить", 650, 200, 200, 50)) checkAnswer();
    if (ui.button(L"В меню", 650, 270, 200, 50)) app.go(ScreenId::Menu);
    ui.text(L"Клик по клетке, затем цифра 1–9\nBackspace — стереть", 600, 360, 18, kGray);
    ui.text(hint, 600, 440, 20, hintColor);
}

void GameScreen::drawWinDialog() {
    Ui& ui = app.ui;
    sf::RectangleShape box({520, 260});
    box.setPosition(190, 180);
    box.setFillColor(sf::Color(250, 250, 255));
    box.setOutlineColor(sf::Color::Black);
    box.setOutlineThickness(3);
    ui.window.draw(box);
    ui.text(L"Всё верно!", 340, 205, 38, kGreen);
    ui.text(sf::String(L"Очки: ") + std::to_string(finalScore) + L"   Время: " + formatTime(finalSeconds), 270, 275, 26);
    if (ui.button(L"В меню", 220, 350, 220, 50)) app.go(ScreenId::Menu);
    if (ui.button(L"Сыграть ещё", 460, 350, 220, 50)) newGame();
}

void GameScreen::draw() {
    handleGridClick();
    drawGrid();
    int secs = solved ? finalSeconds : (int)timer.getElapsedTime().asSeconds();
    app.ui.text(sf::String(L"Время: ") + formatTime(secs), 40, 630, 32);  // таймер слева внизу
    if (solved) drawWinDialog();
    else drawSidePanel();
}
