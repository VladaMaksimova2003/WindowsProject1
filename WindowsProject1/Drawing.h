#pragma once

#include "Config.h"
#include "framework.h"
class Drawing {
public:
    static void DrawEllipse(HDC hdc, int x, int y, int cellWidth, int cellHeight);
    static void DrawCross(HDC hdc, int x, int y, int cellWidth, int cellHeight);
    static void DrawLine(HDC hdc, int x, int y, int sx, int sy, int cellWidth, int cellHeight);
    static void DrawGrid(HDC hdc, int N, int cellWidth, int cellHeight, const std::vector<std::vector<char>>& grid);
};