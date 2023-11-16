#include "Drawing.h"

// Реализация функции отрисовки эллипса
void Drawing::DrawEllipse(HDC hdc, int x, int y, int cellWidth, int cellHeight) {
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 250, 0));
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 255));

    SelectObject(hdc, hPen);
    SelectObject(hdc, hBrush);

    Ellipse(hdc, x - cellWidth / 2, y - cellHeight / 2, x + cellWidth / 2, y + cellHeight / 2);

    DeleteObject(hPen);
    DeleteObject(hBrush);
}

// Реализация функции отрисовки креста
void Drawing::DrawCross(HDC hdc, int x, int y, int cellWidth, int cellHeight) {
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 250, 255));
    SelectObject(hdc, hPen);

    MoveToEx(hdc, x, y, NULL);
    LineTo(hdc, x + cellWidth, y + cellHeight);

    MoveToEx(hdc, x, y + cellHeight, NULL);
    LineTo(hdc, x + cellWidth, y);

    DeleteObject(hPen);
}

// Реализация функции отрисовки линий
void Drawing::DrawLine(HDC hdc, int x, int y, int sx, int sy, int cellWidth, int cellHeight) {
    for (int x = 0; x <= sx; x += cellWidth)
    {
        MoveToEx(hdc, x, 0, NULL);
        LineTo(hdc, x, sy);

    }

    for (int y = 0; y <= sy; y += cellHeight)
    {
        MoveToEx(hdc, 0, y, NULL);
        LineTo(hdc, sx, y);
    }
}

// Реализация функции отрисовки сетки
void Drawing::DrawGrid(HDC hdc, int N, int cellWidth, int cellHeight, const std::vector<std::vector<char>>& grid) {
    int x, y;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (grid[i][j] == 'X') {
                x = i * cellWidth;
                y = j * cellHeight;
                DrawCross(hdc, x, y, cellWidth, cellHeight);
            }

            if (grid[i][j] == 'O') {
                x = i * cellWidth + cellWidth / 2;
                y = j * cellHeight + cellHeight / 2;
                DrawEllipse(hdc, x, y, cellWidth, cellHeight);
            }
        }
    }
}