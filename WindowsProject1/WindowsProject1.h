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
	static HPEN hpen;
	static HBRUSH Brush;
	static int cellWidth, cellHeight;
	static PAINTSTRUCT ps;
	static ConfigManager configManager;
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

