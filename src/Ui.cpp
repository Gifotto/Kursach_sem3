#include "Ui.h"
#include <algorithm>
#include <cstdio>

std::string formatTime(int s) {
    char b[16];
    std::snprintf(b, sizeof b, "%02d:%02d", s / 60, s % 60);
    return b;
}

sf::String difficultyLabel(const std::string& name) {
    if (name == "easy") return L"Лёгкая";
    if (name == "medium") return L"Средняя";
    if (name == "hard") return L"Сложная";
    return name;
}

Ui::Ui() : window(sf::VideoMode(900, 700), L"Судоку", sf::Style::Titlebar | sf::Style::Close) {
    window.setFramerateLimit(60);
}

bool Ui::loadFont() {
    // font.ttf рядом с программой имеет приоритет, затем системные шрифты
    const char* paths[] = {"font.ttf", "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "C:/Windows/Fonts/arial.ttf", "/System/Library/Fonts/Supplemental/Arial.ttf"};
    for (auto p : paths)
        if (font.loadFromFile(p)) return true;
    return false;
}

void Ui::resetInput() { clicked = false; enterPressed = false; wheel = 0; }

void Ui::beginFrame() {
    auto p = sf::Mouse::getPosition(window);
    mouse = {(float)p.x, (float)p.y};
    order.clear();
}

void Ui::text(const sf::String& s, float x, float y, int size, sf::Color col) {
    sf::Text t(s, font, size);
    t.setFillColor(col);
    t.setPosition(x, y);
    window.draw(t);
}

bool Ui::button(const sf::String& s, float x, float y, float w, float h) {
    sf::FloatRect r(x, y, w, h);
    bool hover = r.contains(mouse);
    sf::RectangleShape b({w, h});
    b.setPosition(x, y);
    b.setFillColor(hover ? sf::Color(170, 200, 240) : sf::Color(210, 225, 245));
    b.setOutlineColor(sf::Color(80, 100, 140));
    b.setOutlineThickness(1);
    window.draw(b);
    sf::Text t(s, font, 20);
    t.setFillColor(sf::Color::Black);
    auto bb = t.getLocalBounds();  // центрируем подпись
    t.setPosition(x + (w - bb.width) / 2 - bb.left, y + (h - bb.height) / 2 - bb.top);
    window.draw(t);
    if (hover && clicked) { clicked = false; return true; }  // клик «съедается»
    return false;
}

void Ui::field(Field& f, float x, float y, float w) {
    order.push_back(&f);
    sf::FloatRect r(x, y, w, 32);
    if (clicked && r.contains(mouse)) focus = &f;
    sf::RectangleShape b({w, 32});
    b.setPosition(x, y);
    b.setFillColor(sf::Color::White);
    b.setOutlineThickness(2);
    b.setOutlineColor(focus == &f ? kBlue : kGray);
    window.draw(b);
    text(f.password && !f.revealed ? std::string(f.text.size(), '*') : f.text, x + 6, y + 2, 20);
}

void Ui::passwordField(Field& f, float x, float y, float w) {
    field(f, x, y, w);
    // кнопка справа от поля переключает видимость пароля
    if (button(f.revealed ? L"Скрыть" : L"Показать", x + w + 10, y, 110, 32)) f.revealed = !f.revealed;
}

bool Ui::rowBox(float x, float y, float w, bool selected) {
    sf::FloatRect r(x, y, w, 28);
    sf::RectangleShape b({w, 28});
    b.setPosition(x, y);
    b.setFillColor(selected ? sf::Color(190, 215, 250)
        : (r.contains(mouse) ? sf::Color(235, 240, 250) : sf::Color(248, 248, 248)));
    window.draw(b);
    if (clicked && r.contains(mouse)) { clicked = false; return true; }
    return false;
}

void Ui::typeChar(uint32_t c) {
    if (!focus) return;
    if (c == 8) { if (!focus->text.empty()) focus->text.pop_back(); }  // Backspace
    else if (c > 32 && c < 127 && focus->text.size() < focus->maxLen) focus->text += (char)c;
}

void Ui::nextField() {
    if (order.empty()) return;
    auto it = std::find(order.begin(), order.end(), focus);
    size_t i = it == order.end() ? 0 : (it - order.begin() + 1) % order.size();
    focus = order[i];
}
