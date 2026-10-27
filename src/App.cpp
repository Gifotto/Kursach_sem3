#include "App.h"
#include "AdminScreen.h"
#include "GameScreen.h"
#include "LoginScreen.h"
#include "MenuScreen.h"
#include "RecordsScreen.h"
#include "RegisterScreen.h"

App::App() {
    screens[(int)ScreenId::Login] = std::make_unique<LoginScreen>(*this);
    screens[(int)ScreenId::Register] = std::make_unique<RegisterScreen>(*this);
    screens[(int)ScreenId::Menu] = std::make_unique<MenuScreen>(*this);
    screens[(int)ScreenId::Records] = std::make_unique<RecordsScreen>(*this);
    screens[(int)ScreenId::Game] = std::make_unique<GameScreen>(*this);
    screens[(int)ScreenId::Admin] = std::make_unique<AdminScreen>(*this);
}

void App::setMessage(const sf::String& s, bool error) {
    message = s;
    messageColor = error ? kRed : kGreen;
}

void App::drawMessage(float x, float y) { ui.text(message, x, y, 20, messageColor); }

void App::go(ScreenId id) {
    current = id;
    ui.focus = nullptr;
    message = "";
    screens[(int)id]->onEnter();
}

void App::logout() {
    userId = 0;
    userName.clear();
    go(ScreenId::Login);
}

void App::handleEvents() {
    sf::Event ev;
    while (ui.window.pollEvent(ev)) {
        switch (ev.type) {
        case sf::Event::Closed: ui.window.close(); break;
        case sf::Event::MouseButtonPressed:
            if (ev.mouseButton.button == sf::Mouse::Left) ui.clicked = true;
            break;
        case sf::Event::MouseWheelScrolled:
            ui.wheel = ev.mouseWheelScroll.delta;
            ui.wheelX = (float)ev.mouseWheelScroll.x;
            break;
        case sf::Event::KeyPressed:
            if (ev.key.code == sf::Keyboard::Return) ui.enterPressed = true;
            if (ev.key.code == sf::Keyboard::Tab) ui.nextField();
            break;
        case sf::Event::TextEntered:
            ui.typeChar(ev.text.unicode);                       // в активное поле ввода
            screens[(int)current]->onText(ev.text.unicode);   // и текущему экрану (цифры судоку)
            break;
        default: break;
        }
    }
}

int App::run() {
    if (!ui.loadFont()) return 1;  // положите рядом font.ttf с кириллицей
    bool connected = db.connect();
    go(ScreenId::Login);
    if (!connected) setMessage(L"Нет соединения с БД. Запустите: docker compose up -d");

    while (ui.window.isOpen()) {
        ui.resetInput();
        handleEvents();
        ui.beginFrame();
        ui.window.clear(sf::Color(245, 245, 250));
        screens[(int)current]->draw();
        ui.window.display();
    }
    return 0;
}
