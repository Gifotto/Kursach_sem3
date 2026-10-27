#pragma once
#include "App.h"

// Экран входа в аккаунт (и пользователь, и администратор входят здесь).
class LoginScreen : public Screen {
public:
    explicit LoginScreen(App& apps) : app(apps) { nick.maxLen = 20; pass.password = true; }
    void onEnter() override;
    void draw() override;
private:
    void tryLogin();
    App& app;
    Field nick, pass;
};
