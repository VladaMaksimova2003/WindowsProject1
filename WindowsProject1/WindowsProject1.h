#pragma once

#include "Resource.h"
#include "Config.h"

class MyWinApp {
public:
    MyWinApp(HINSTANCE hInstance);
    ~MyWinApp();

    int Run();


private:
    HINSTANCE hInstance;
    HWND hWnd;
    static int sx, sy, x, y;
    static HPEN hpen;
    static HBRUSH Brush;
    static STARTUPINFO tin;
    static PROCESS_INFORMATION pInfo;
    static int cellWidth, cellHeight;
    static PAINTSTRUCT ps;
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
