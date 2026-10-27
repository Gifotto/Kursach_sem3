#pragma once
#include "App.h"

// Меню пользователя + общая таблица рекордов (топ-5).
class MenuScreen : public Screen {
public:
    explicit MenuScreen(App& apps) : app(apps) {}
    void onEnter() override;
    void draw() override;
private:
    App& app;
    Rows top;                        // [ник, очки, время]
    std::vector<Difficulty> levels;  // уровни сложности из БД
};
