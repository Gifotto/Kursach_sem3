#pragma once
#include "App.h"

// Панель администратора: пользователи и их рекорды, редактирование,
// создание пользователей и партий, смена паролей, удаление.
class AdminScreen : public Screen {
public:
    explicit AdminScreen(App& apps) : app(apps) {}
    void onEnter() override;
    void draw() override;
private:
    void reload();         // перечитать таблицы из БД
    void loadDetails();    // рекорды и журнал входов выбранного пользователя
    void drawLists();      // две таблицы со скроллом
    void drawControls();   // поля ввода и кнопки действий

    void changeNick();
    void changePassword();
    void changeScore();
    void changeTime();
    void createUser();
    void addGame();

    App& app;
    Rows users, games;             // [id, ник, лучший счёт, игр] / [id, очки, время]
    Rows loginRows;                // журнал входов выбранного: [id, время]
    bool showLog = false;          // справа показан журнал входов вместо рекордов
    std::vector<Difficulty> levels; // уровни сложности для новых партий
    size_t levelIdx = 0;            // выбранный уровень (кнопка «Сложность»)
    int selUser = 0, selGame = 0;  // выбранные id (0 — ничего)
    int scrollUsers = 0, scrollGames = 0;
    Field valueA, valueB;          // поля ввода: значения для действий (см. подсказку на экране)
};