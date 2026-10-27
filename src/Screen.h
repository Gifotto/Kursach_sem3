#pragma once
#include <cstdint>

// Базовый класс экрана приложения. Каждый экран — отдельный класс-наследник.
class Screen {
public:
    virtual ~Screen() = default;
    virtual void onEnter() {}            // вызывается при переходе на экран (загрузка данных, сброс полей)
    virtual void onText(uint32_t) {}     // ввод символа с клавиатуры (нужен игровому экрану)
    virtual void draw() = 0;             // отрисовка кадра и обработка кликов
};
