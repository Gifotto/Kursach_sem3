#pragma once
#include "App.h"
#include "SudokuBoard.h"

// Игровой экран: сетка, таймер, проверка решения и окно победы.
class GameScreen : public Screen {
public:
    explicit GameScreen(App& apps) : app(apps) {}
    void onEnter() override { newGame(); }
    void onText(uint32_t c) override;  // ввод цифр в выбранную клетку
    void draw() override;
private:
    void newGame();
    void checkAnswer();
    void handleGridClick();
    void drawGrid();
    void drawSidePanel();
    void drawWinDialog();

    static constexpr float kGridX = 40, kGridY = 30, kCell = 60;  // положение и размер сетки

    App& app;
    SudokuBoard board;
    sf::Clock timer;
    int selRow = -1, selCol = -1;  // выбранная клетка
    bool solved = false;
    int finalSeconds = 0, finalScore = 0;
    sf::String hint;                // сообщение под кнопками (ошибки / «не всё заполнено»)
    sf::Color hintColor = kRed;
};
