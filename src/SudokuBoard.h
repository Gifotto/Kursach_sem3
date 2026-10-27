#pragma once
#include <random>

// Игровое поле судоку: генерация, ввод цифр пользователем и проверка.
class SudokuBoard {
public:
    enum class CheckResult { Errors, Incomplete, Solved };

    // Создаёт новую случайную головоломку с единственным решением (holes — число пустых клеток).
    void generate(int holes = 45);

    int value(int r, int c) const { return cur[r][c]; }      // текущая цифра (0 — пусто)
    bool isGiven(int r, int c) const { return given[r][c]; } // цифра была в условии (чёрная)
    bool isWrong(int r, int c) const { return wrong[r][c]; } // цифра помечена как ошибочная

    // Записывает цифру 1–9 (0 — стереть). Исходные клетки изменить нельзя.
    void setValue(int r, int c, int v);

    // Сравнивает ввод с решением и помечает ошибки.
    CheckResult check();

private:
    int solution[9][9] = {};  // полное решение
    int puzzle[9][9] = {};    // условие (с пустыми клетками)
    int cur[9][9] = {};       // то, что сейчас на поле
    bool given[9][9] = {};
    bool wrong[9][9] = {};
    std::mt19937 rng{std::random_device{}()};

    static bool canPlace(int g[9][9], int r, int c, int v);  // можно ли поставить v по правилам
    bool fill(int g[9][9], int pos);                         // заполнение сетки перебором со случайным порядком
    int countSolutions(int g[9][9], int pos, int limit);     // число решений (не больше limit)
};
