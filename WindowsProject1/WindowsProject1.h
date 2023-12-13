#pragma once

#include "Resource.h"
#include "Config.h"
#include "framework.h"

class MyWinApp {
public:
	MyWinApp(HINSTANCE hInstance);
	~MyWinApp();
	int Run();
	int GetCellWidth() const {
		return cellWidth;
	}
	int GetCellHeight() const {
		return cellHeight;
	}
	HPEN GetHpen() const {
		return hpen;
	}

private:
	HINSTANCE hInstance;
	HWND hWnd;
	static HPEN hpen;
	static HBRUSH Brush;
	static int cellWidth;
	static int cellHeight;
	static PAINTSTRUCT ps;
	static ConfigManager configManager;
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static void GettingData(HWND hWnd, UINT message, MyWinApp** pApp, LPARAM lParam);
};
struct InfoPlayers {
	DWORD ProcessId;
	int PlayerNumber;
};
struct MoveHistory {
	int x;
	int y;
	int value;
};
struct SharedData {
	int currentPlayer = 0;
	int setPlayer = 1;
	InfoPlayers infoPlayers[NUNBER_OF_PLAYERS];
	MoveHistory moveHistory[MAX_MOVES][MAX_MOVES];
	SharedData() : moveHistory{} {}

	void addMove(int x, int y, int value, int index1, int index2) {
		moveHistory[index1][index2].x = x;
		moveHistory[index1][index2].y = y;
		moveHistory[index1][index2].value = value;
	}
};
