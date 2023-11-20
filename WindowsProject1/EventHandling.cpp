#include "EventHandling.h"


EventHandling::EventHandling() {
    CommandLine[0] = L'\0';
    ZeroMemory(&pInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&tin, sizeof(STARTUPINFO));
}

EventHandling::~EventHandling() {}

void EventHandling::HandleMouseWheel(WPARAM wParam, COLORREF& gridColor) {
    int r = GetRValue(gridColor);
    int g = GetGValue(gridColor);
    int b = GetBValue(gridColor);
    int difference = GET_WHEEL_DELTA_WPARAM(wParam);

    if (difference > 0) {
        if (r < 250) r += 5;
        else if (g < 250) g += 5;
        else if (b < 250) b += 5;
        else r = g = b = 0;
    }
    else if (difference < 0) {
        if (r > 0) r -= 5;
        else if (g > 0) g -= 5;
        else if (b > 0) b -= 5;
        else r = g = b = 255;
    }

    gridColor = RGB(r, g, b);
}

void EventHandling::HandleRightMouseClick(HWND hWnd, LPARAM lParam, int cellWidth, int cellHeight, int N, int* p, int* q) {
    int k, m;
    int x = LOWORD(lParam) / cellWidth;
    int y = HIWORD(lParam) / cellHeight;

    if (x >= 0 && x < N && y >= 0 && y < N) {
        for (k = 0, q = p; k < N; k++)
            for (m = 0; m < N; m++, q++) {
                if (k == x && m == y) {
                    *q = (*q == 1) ? 0 : 1;
                    break;
                }
            }
    }
}

void EventHandling::HandleLeftMouseClick(HWND hWnd, LPARAM lParam, int cellWidth, int cellHeight, int N, int* p, int* q) {
    int k, m;
    int x = LOWORD(lParam) / cellWidth;
    int y = HIWORD(lParam) / cellHeight;

    if (x >= 0 && x < N && y >= 0 && y < N) {
        for (k = 0, q = p; k < N; k++)
            for (m = 0; m < N; m++, q++) {
                if (k == x && m == y) {
                    *q = (*q == 2) ? 0 : 2;
                    break;
                }
            }
    }
}

void EventHandling::HandleHotKey(HWND hWnd, WPARAM wParam) {
    if (wParam == 0) {
        DestroyWindow(hWnd);
    }

    if (wParam == 1) {
        if (CreateProcess(NULL, CommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &tin, &pInfo)) {
            CloseHandle(pInfo.hProcess);
            CloseHandle(pInfo.hThread);
        }
        else {
            MessageBox(NULL, L"Не удалось запустить блокнот.", L"Ошибка", MB_OK | MB_ICONERROR);
        }
    }
}

