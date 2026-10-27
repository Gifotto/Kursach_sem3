#include "Validator.h"
#include <cctype>

bool Validator::isAsciiVisible(const std::string& s) {
    for (unsigned char c : s)
        if (c < 33 || c > 126) return false;
    return true;
}

std::wstring Validator::checkNick(const std::string& nick) {
    if (nick.size() < 3 || nick.size() > 20) return L"Ник: от 3 до 20 символов";
    if (!isAsciiVisible(nick)) return L"Ник: только латиница, цифры и спецсимволы";
    return L"";
}

std::wstring Validator::checkPassword(const std::string& pass, const std::string& repeat) {
    if (pass.size() < 8) return L"Пароль: не менее 8 символов";
    if (!isAsciiVisible(pass)) return L"Пароль: только латиница, цифры и спецсимволы";
    bool letter = false, digit = false, special = false;
    for (unsigned char c : pass) {
        if (std::isalpha(c)) letter = true;
        else if (std::isdigit(c)) digit = true;
        else special = true;
    }
    if (!letter || !digit || !special) return L"Пароль: нужны латиница, цифра и спецсимвол";
    if (pass != repeat) return L"Пароли не совпадают";
    return L"";
}

bool Validator::parseInt(const std::string& s, int& out) {
    if (s.empty() || s.size() > 9) return false;
    for (char c : s)
        if (!std::isdigit(static_cast<unsigned char>(c))) return false;
    out = std::stoi(s);
    return true;
}
