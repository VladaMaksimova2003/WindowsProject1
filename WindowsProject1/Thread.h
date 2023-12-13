#pragma once
#include "framework.h"
#include "WindowsProject1.h"
#include "Drawer.h"
struct ThreadParams {
	HWND hWnd;
	MyWinApp* pApp;
	int N;
	int* p;
	int* q;
	SharedData* pData;
};

class Thread
{
public:
	static bool isThreadActive;
	static bool isThreadPaused;
	static ThreadParams* params;
public:
	void StartThread();
	void StopThread();
	void ResumeThreadExecution();
	void SuspendThreadExecution();
	static void setThreadPriority(int priority);
	ThreadParams* InitializationThreadParams(HWND hWnd, MyWinApp* pApp, ConfigManager& configManager, int* p, int* q, SharedData* pData);
	static void ChangeGradientColor(HWND hwnd);
	static COLORREF GetGradientColor(int x, int y, int width, int height);
	static COLORREF HSLtoRGB(double h, double s, double l);
	static DWORD WINAPI ThreadFunctionCircle(LPVOID lpParam);
	static DWORD WINAPI ThreadFunctionBackground(LPVOID lpParam);
};