#pragma once

#include <Windows.h>
#include "Config.h"
#include "framework.h"

class EventHandling {
public:
    EventHandling();
    ~EventHandling();

    void HandleMouseWheel(WPARAM wParam, COLORREF& gridColor);
    void HandleRightMouseClick(HWND hWnd, LPARAM lParam, int cellWidth, int cellHeight, int N, int* p, int* q);
    void HandleLeftMouseClick(HWND hWnd, LPARAM lParam, int cellWidth, int cellHeight, int N, int* p, int* q);
    void HandleHotKey(HWND hWnd, WPARAM wParam);

private:
    TCHAR CommandLine[256];
    PROCESS_INFORMATION pInfo;
    STARTUPINFO tin;
};

