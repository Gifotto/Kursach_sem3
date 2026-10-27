#pragma once
#include <memory>
#include <string>
#include "Database.h"
#include "Screen.h"
#include "Ui.h"

enum class ScreenId { Login, Register, Menu, Records, Game, Admin, Count };

// Приложение: владеет окном, базой, текущим пользователем и экранами; крутит главный цикл.
class App {
public:
    App();
    int run();                           // главный цикл; возвращает код выхода

    void go(ScreenId id);                // переход на экран
    void logout();                       // выход из аккаунта -> экран входа
    void quit() { ui.window.close(); }   // выход из игры

    // Сообщение пользователю (ошибка — красным, успех — зелёным)
    void setMessage(const sf::String& s, bool error = true);
    void drawMessage(float x, float y);

    Ui ui;
    Database db;
    int userId = 0;                      // текущий пользователь (0 — не вошёл)
    std::string userName;
    Difficulty difficulty;               // выбранный уровень сложности

private:
    void handleEvents();

    std::unique_ptr<Screen> screens[(int)ScreenId::Count];
    ScreenId current = ScreenId::Login;
    sf::String message;
    sf::Color messageColor = kRed;
};
