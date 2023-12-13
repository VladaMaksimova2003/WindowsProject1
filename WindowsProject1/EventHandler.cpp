#include "EventHandler.h"

EventHandler::EventHandler() {
	_tcscpy_s(CommandLine, _T("notepad"));
	ZeroMemory(&pInfo, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&tin, sizeof(STARTUPINFO));
}

EventHandler::~EventHandler() {}

void EventHandler::HandleMouseWheel(WPARAM wParam, COLORREF& gridColor) {
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

//void EventHandler::HandleRightMouseClick(HWND hWnd, LPARAM lParam, int cellWidth, int cellHeight, SharedData* sharedData) {
//	int x = LOWORD(lParam) / cellWidth;
//	int y = HIWORD(lParam) / cellHeight;
//	int k, m;
//	int* p = sharedData->arrayInfo;
//	if (x >= 0 && x < sharedData->ArraySize && y >= 0 && y < sharedData->ArraySize) {
//		for (k = 0; k < sharedData->ArraySize; k++)
//			for (m = 0; m < sharedData->ArraySize; m++, p++) {
//				if (k == x && m == y) {
//					*p = (*p == 1) ? 0 : 1;
//					break;
//				}
//			}
//	}
//}

void EventHandler::HandleRightMouseClick(HWND hWnd, LPARAM lParam, int cellWidth, int cellHeight, int N, int* p, int* q, SharedData* sharedData) {
	int k, m;
	int x = LOWORD(lParam) / cellWidth;
	int y = HIWORD(lParam) / cellHeight;

	if (x >= 0 && x < N && y >= 0 && y < N) {
		for (k = 0, q = p; k < N; k++)
			for (m = 0; m < N; m++, q++) {
				if (k == x && m == y) {
					*q = (*q == 1) ? 0 : 1;
					sharedData->addMove(x, y, *q, k, m);
					break;
				}
			}
	}
}

void EventHandler::HandleLeftMouseClick(HWND hWnd, LPARAM lParam, int cellWidth, int cellHeight, int N, int* p, int* q, SharedData* sharedData) {
	int k, m;
	int x = LOWORD(lParam) / cellWidth;
	int y = HIWORD(lParam) / cellHeight;

	if (x >= 0 && x < N && y >= 0 && y < N) {
		for (k = 0, q = p; k < N; k++)
			for (m = 0; m < N; m++, q++) {
				if (k == x && m == y) {
					*q = (*q == 2) ? 0 : 2;
					sharedData->addMove(x, y, *q, k, m);
					break;
				}
			}
	}
}
//void EventHandler::HandleLeftMouseClick(HWND hWnd, LPARAM lParam, int cellWidth, int cellHeight, SharedData* sharedData) {
//    int x = LOWORD(lParam) / cellWidth;
//    int y = HIWORD(lParam) / cellHeight;
//    int k, m;
//    if (x >= 0 && x < sharedData->ArraySize && y >= 0 && y < sharedData->ArraySize) {
//        for (k = 0; k < sharedData->ArraySize; k++)
//            for (m = 0; m < sharedData->ArraySize; m++) {
//                if (k == x && m == y) {
//                    sharedData->pdata[k][m] = (sharedData->pdata[k][m] == 2) ? 0 : 2;
//                    break;
//                }
//            }
//    }
//}

//void EventHandler::HandleLeftMouseClick(HWND hWnd, LPARAM lParam, int cellWidth, int cellHeight, SharedData* sharedData) {
//	int x = LOWORD(lParam) / cellWidth;
//	int y = HIWORD(lParam) / cellHeight;
//	int k, m;
//	int* p = sharedData->arrayInfo;
//	if (x >= 0 && x < sharedData->ArraySize && y >= 0 && y < sharedData->ArraySize) {
//		for (k = 0; k < sharedData->ArraySize; k++)
//			for (m = 0; m < sharedData->ArraySize; m++, p++) {
//				if (k == x && m == y) {
//					*p = (*p == 2) ? 0 : 2;
//					break;
//				}
//			}
//	}
//}

void EventHandler::HandleHotKey(HWND hWnd, WPARAM wParam) {
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

