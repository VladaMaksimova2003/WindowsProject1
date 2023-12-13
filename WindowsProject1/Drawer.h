#pragma once

#include "Config.h"
#include "framework.h"
#include "WindowsProject1.h"
class Drawer {
public:
    void DrawEllipse(HDC hdc, int x, int y, int cellWidth, int cellHeight);
    void DrawCross(HDC hdc, int x, int y, int cellWidth, int cellHeight);
    void DrawLine(HDC hdc, int cellWidth, int cellHeight);
    void DrawHorizonalLine(HDC hdc, int cellWidth, int cellHeight);
    void DrawVerticalLine(HDC hdc, int cellWidth, int cellHeight);
    //void DrawGrid(HDC hdc, int cellWidth, int cellHeight, SharedData* sharedData);
    void DrawGrid(HDC hdc, int N, int cellWidth, int cellHeight, int* p, int* q);
    void DrawFigure(HDC hdc, int cellWidth, int cellHeight, int i, int j, int value);
    void SetSize(int sx, int sy);

private:
    static int sx;
    static int sy;
};
