#pragma once
#include <SFML/Graphics.hpp>
#include <cstdint>
#include <string>
#include <vector>

// Цвета интерфейса
inline const sf::Color kBlue(30, 70, 220);
inline const sf::Color kRed(220, 30, 30);
inline const sf::Color kGray(110, 110, 110);
inline const sf::Color kGreen(20, 130, 20);

// Текстовое поле ввода (данные хранит сам экран, рисует Ui).
struct Field {
    std::string text;
    bool password = false;  // поле пароля (по умолчанию скрыто звёздочками)
    bool revealed = false;  // пароль временно показан открытым текстом
    size_t maxLen = 32;
};

std::string formatTime(int seconds);  // 125 -> "02:05"
sf::String difficultyLabel(const std::string& name);  // easy -> "Лёгкая"

// Окно и простые элементы интерфейса (режим "immediate mode":
// кнопка рисуется и сразу возвращает true, если по ней кликнули).
class Ui {
public:
    Ui();
    bool loadFont();                // ищет шрифт с кириллицей
    void resetInput();              // сброс событий в начале кадра
    void beginFrame();              // обновляет позицию мыши и список полей

    void text(const sf::String& s, float x, float y, int size = 22, sf::Color col = sf::Color::Black);
    bool button(const sf::String& s, float x, float y, float w, float h);
    void field(Field& f, float x, float y, float w);
    void passwordField(Field& f, float x, float y, float w);  // поле + кнопка «Показать/Скрыть»
    bool rowBox(float x, float y, float w, bool selected);  // строка таблицы; true при клике

    void typeChar(uint32_t c);      // ввод символа в активное поле
    void nextField();               // Tab

    sf::RenderWindow window;
    sf::Font font;
    sf::Vector2f mouse;
    bool clicked = false;           // был клик левой кнопкой в этом кадре
    bool enterPressed = false;
    float wheel = 0, wheelX = 0;    // прокрутка колеса и позиция мыши при ней
    Field* focus = nullptr;         // активное поле
private:
    std::vector<Field*> order;     // поля текущего экрана (для Tab)
};
