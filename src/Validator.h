#pragma once
#include <string>

// Проверка данных, вводимых пользователем (ник, пароль, числа).
class Validator {
public:
    // Возвращает текст ошибки или пустую строку, если ник допустим.
    static std::wstring checkNick(const std::string& nick);
    // Проверяет пароль и его повтор; пустая строка = всё в порядке.
    static std::wstring checkPassword(const std::string& pass, const std::string& repeat);
    // Разбирает неотрицательное целое (до 9 цифр). false, если строка некорректна.
    static bool parseInt(const std::string& s, int& out);
private:
    // true, если все символы — печатные ASCII без пробела (латиница, цифры, спецсимволы).
    static bool isAsciiVisible(const std::string& s);
};
