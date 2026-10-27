#include "RegisterScreen.h"
#include "Validator.h"

void RegisterScreen::onEnter() {
    nick.text.clear(); pass.text.clear(); repeat.text.clear();
    pass.revealed = repeat.revealed = false;
    app.ui.focus = &nick;
}

void RegisterScreen::tryRegister() {
    // Сначала проверяем формат ника и пароля, затем уникальность ника в БД
    std::wstring err = Validator::checkNick(nick.text);
    if (err.empty()) err = Validator::checkPassword(pass.text, repeat.text);
    if (!err.empty()) { app.setMessage(err); return; }

    switch (app.db.registerUser(nick.text, pass.text)) {
    case RegisterResult::Taken: app.setMessage(L"Такой ник уже занят"); break;
    case RegisterResult::Error: {
        // показываем причину из PostgreSQL (первая строка, с переносом по 45 символов)
        std::string e = app.db.lastError();
        e = e.substr(0, e.find('\n'));
        if (e.size() > 90) e.resize(90);
        for (size_t i = 45; i < e.size(); i += 46) e.insert(i, "\n");
        app.setMessage(sf::String(L"Не удалось зарегистрировать:\n") + e);
        break;
    }
    case RegisterResult::Ok:
        app.go(ScreenId::Login);
        app.setMessage(L"Регистрация успешна, войдите", false);
        break;
    }
}

void RegisterScreen::draw() {
    Ui& ui = app.ui;
    ui.text(L"Регистрация", 340, 70, 40);
    ui.text(L"Ник (латиница, цифры, спецсимволы, 3–20):", 250, 150, 20);   ui.field(nick, 250, 180, 400);
    ui.text(L"Пароль (от 8: латиница + цифра + спецсимвол):", 250, 230, 20); ui.passwordField(pass, 250, 260, 400);
    ui.text(L"Повторите пароль:", 250, 310, 20); ui.passwordField(repeat, 250, 340, 400);
    if (ui.button(L"Создать аккаунт", 250, 410, 190, 42) || ui.enterPressed) tryRegister();
    if (ui.button(L"Назад", 460, 410, 190, 42)) app.go(ScreenId::Login);
    app.drawMessage(250, 470);
}
