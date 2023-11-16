#pragma once

#include <Windows.h>
#include "Config.h"
#include "framework.h"

class EventHandling {
public:
    static void HandleMouseWheel(HWND hWnd, WPARAM wParam, COLORREF& gridColor);
    static void HandleRightMouseClick(HWND hWnd, LPARAM lParam, int cellWidth, int cellHeight, int N, std::vector<std::vector<char>>& grid);
    static void HandleLeftMouseClick(HWND hWnd, LPARAM lParam, int cellWidth, int cellHeight, int N, std::vector<std::vector<char>>& grid);
    static void HandleHotKey(HWND hWnd, WPARAM wParam, STARTUPINFO tin, PROCESS_INFORMATION pInfo);

public:
    static TCHAR CommandLine[256];
};