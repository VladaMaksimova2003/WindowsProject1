#pragma once
#pragma once

#include "Config.h"
#include "framework.h"
class Drawing {
public:
    void DrawEllipse(HDC hdc, int x, int y, int cellWidth, int cellHeight);
    void DrawCross(HDC hdc, int x, int y, int cellWidth, int cellHeight);
    void DrawLine(HDC hdc, int cellWidth, int cellHeight);
    void DrawGrid(HDC hdc, int N, int cellWidth, int cellHeight, int* p, int* q);
    void SetSize(int sx, int sy);

private:
    static int sx;
    static int sy;
};
