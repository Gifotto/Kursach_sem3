#pragma once
#include "App.h"

// Вкладка «Мои рекорды» — лучшие партии текущего пользователя.
class RecordsScreen : public Screen {
public:
    explicit RecordsScreen(App& apps) : app(apps) {}
    void onEnter() override;
    void draw() override;
private:
    App& app;
    Rows records;  // [очки, время, дата, сложность]
};
