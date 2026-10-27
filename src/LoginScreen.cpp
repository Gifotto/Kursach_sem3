#include "LoginScreen.h"

void LoginScreen::onEnter() {
    pass.text.clear();
    pass.revealed = false;
    app.ui.focus = &nick;
}

void LoginScreen::tryLogin() {
    LoginInfo info = app.db.login(nick.text, pass.text);
    if (info.dbError) { app.setMessage(L"Ошибка базы данных (запущен ли Docker?)"); return; }
    if (!info.ok) { app.setMessage(L"Неверный ник или пароль"); return; }
    app.userId = info.id;
    app.userName = info.nickname;
    app.go(info.isAdmin ? ScreenId::Admin : ScreenId::Menu);  // админ попадает в панель администратора
}

void LoginScreen::draw() {
    Ui& ui = app.ui;
    ui.text(L"СУДОКУ", 340, 80, 56);
    ui.text(L"Вход в аккаунт", 330, 170, 26);
    ui.text(L"Ник:", 250, 230);    ui.field(nick, 250, 260, 400);
    ui.text(L"Пароль:", 250, 310); ui.passwordField(pass, 250, 340, 400);
    if (ui.button(L"Войти", 250, 400, 190, 42) || ui.enterPressed) tryLogin();
    if (ui.button(L"Регистрация", 460, 400, 190, 42)) app.go(ScreenId::Register);
    if (ui.button(L"Выйти из игры", 330, 580, 240, 42)) app.quit();
    app.drawMessage(250, 460);
}
