#pragma once

#include <Windows.h>
#include "Config.h"
#include "framework.h"
#include "WindowsProject1.h"
class EventHandler {
public:
    EventHandler();
    ~EventHandler();

    void HandleMouseWheel(WPARAM wParam, COLORREF& gridColor);
    //void HandleLeftMouseClick(HWND hWnd, LPARAM lParam, int cellWidth, int cellHeight, SharedData* sharedData);
    //void HandleRightMouseClick(HWND hWnd, LPARAM lParam, int cellWidth, int cellHeight, SharedData* sharedData);
    void HandleRightMouseClick(HWND hWnd, LPARAM lParam, int cellWidth, int cellHeight, int N, int* p, int* q, SharedData* sharedData);
    void HandleLeftMouseClick(HWND hWnd, LPARAM lParam, int cellWidth, int cellHeight, int N, int* p, int* q, SharedData* sharedData);
    void HandleHotKey(HWND hWnd, WPARAM wParam);

private:
    TCHAR CommandLine[256];
    PROCESS_INFORMATION pInfo;
    STARTUPINFO tin;
};

