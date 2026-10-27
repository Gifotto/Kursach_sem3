#pragma once
#include "App.h"

// Экран регистрации нового пользователя.
class RegisterScreen : public Screen {
public:
    explicit RegisterScreen(App& apps) : app(apps) {
        nick.maxLen = 20; pass.password = true; repeat.password = true;
    }
    void onEnter() override;
    void draw() override;
private:
    void tryRegister();
    App& app;
    Field nick, pass, repeat;
};
