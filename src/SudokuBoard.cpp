#include "SudokuBoard.h"
#include <algorithm>
#include <array>
#include <numeric>
#include <vector>

bool SudokuBoard::canPlace(int g[9][9], int r, int c, int v) {
    for (int i = 0; i < 9; i++)  // строка и столбец
        if (g[r][i] == v || g[i][c] == v) return false;
    int br = r / 3 * 3, bc = c / 3 * 3;  // блок 3×3
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (g[br + i][bc + j] == v) return false;
    return true;
}

bool SudokuBoard::fill(int g[9][9], int pos) {
    if (pos == 81) return true;
    int r = pos / 9, c = pos % 9;
    if (g[r][c]) return fill(g, pos + 1);
    std::array<int, 9> digits;
    std::iota(digits.begin(), digits.end(), 1);
    std::shuffle(digits.begin(), digits.end(), rng);  // случайный порядок => случайная сетка
    for (int v : digits)
        if (canPlace(g, r, c, v)) {
            g[r][c] = v;
            if (fill(g, pos + 1)) return true;
            g[r][c] = 0;  // откат
        }
    return false;
}

int SudokuBoard::countSolutions(int g[9][9], int pos, int limit) {
    while (pos < 81 && g[pos / 9][pos % 9]) pos++;
    if (pos == 81) return 1;
    int r = pos / 9, c = pos % 9, n = 0;
    for (int v = 1; v <= 9 && n < limit; v++)
        if (canPlace(g, r, c, v)) {
            g[r][c] = v;
            n += countSolutions(g, pos + 1, limit);
            g[r][c] = 0;
        }
    return n;
}

void SudokuBoard::generate(int holes) {
    for (auto& row : solution) std::fill(std::begin(row), std::end(row), 0);
    fill(solution, 0);
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++) puzzle[i][j] = solution[i][j];

    // Удаляем клетки в случайном порядке; если решение перестаёт быть единственным — возвращаем цифру
    std::vector<int> order(81);
    std::iota(order.begin(), order.end(), 0);
    std::shuffle(order.begin(), order.end(), rng);
    int removed = 0;
    for (int idx : order) {
        if (removed >= holes) break;
        int r = idx / 9, c = idx % 9, keep = puzzle[r][c];
        puzzle[r][c] = 0;
        if (countSolutions(puzzle, 0, 2) != 1) puzzle[r][c] = keep;
        else removed++;
    }

    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++) {
            cur[i][j] = puzzle[i][j];
            given[i][j] = puzzle[i][j] != 0;
            wrong[i][j] = false;
        }
}

void SudokuBoard::setValue(int r, int c, int v) {
    if (given[r][c]) return;
    cur[r][c] = v;
    wrong[r][c] = false;  // после правки красная отметка снимается
}

SudokuBoard::CheckResult SudokuBoard::check() {
    bool empty = false, bad = false;
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++) {
            wrong[i][j] = false;
            if (!cur[i][j]) empty = true;
            else if (cur[i][j] != solution[i][j]) { wrong[i][j] = true; bad = true; }
        }
    if (bad) return CheckResult::Errors;
    return empty ? CheckResult::Incomplete : CheckResult::Solved;
}
