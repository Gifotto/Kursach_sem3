#include "RecordsScreen.h"

void RecordsScreen::onEnter() { records = app.db.userRecords(app.userId); }

void RecordsScreen::draw() {
    Ui& ui = app.ui;
    ui.text(L"Мои рекорды", 40, 30, 34);
    ui.text(L"#", 40, 90, 20, kGray);      ui.text(L"Очки", 100, 90, 20, kGray);
    ui.text(L"Время", 250, 90, 20, kGray); ui.text(L"Дата", 400, 90, 20, kGray);
    ui.text(L"Сложность", 620, 90, 20, kGray);
    for (size_t i = 0; i < records.size(); i++) {
        float y = 125 + i * 30;
        ui.text(std::to_string(i + 1), 40, y);
        ui.text(records[i][0], 100, y);
        ui.text(formatTime(std::stoi(records[i][1])), 250, y);
        ui.text(records[i][2], 400, y);
        ui.text(difficultyLabel(records[i][3]), 620, y);
    }
    if (records.empty()) ui.text(L"Вы ещё не играли", 40, 130, 20, kGray);
    if (ui.button(L"Назад", 40, 620, 200, 42)) app.go(ScreenId::Menu);
}
