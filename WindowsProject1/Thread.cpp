#include "Thread.h"
bool Thread::isThreadActive = false;
bool Thread::isThreadPaused = false;
int currentPriority = THREAD_PRIORITY_NORMAL;
ThreadParams* Thread::params = nullptr;
HANDLE hThreadBackground;
HANDLE hThreadForeground;
CRITICAL_SECTION cs;
COLORREF Thread::HSLtoRGB(double h, double s, double l) {
	double c = (1.0 - fabs(2.0 * l - 1.0)) * s;
	double x = c * (1.0 - fabs(fmod(h / 60.0, 2.0) - 1.0));
	double m = l - 0.5 * c;

	double r, g, b;

	if (h >= 0 && h < 60) {
		r = c; g = x; b = 0;
	}
	else if (h >= 60 && h < 120) {
		r = x; g = c; b = 0;
	}
	else if (h >= 120 && h < 180) {
		r = 0; g = c; b = x;
	}
	else if (h >= 180 && h < 240) {
		r = 0; g = x; b = c;
	}
	else if (h >= 240 && h < 300) {
		r = x; g = 0; b = c;
	}
	else {
		r = c; g = 0; b = x;
	}

	return RGB(static_cast<int>((r + m) * 255), static_cast<int>((g + m) * 255), static_cast<int>((b + m) * 255));
}
COLORREF Thread::GetGradientColor(int x, int y, int width, int height) {
	// Calculate normalized coordinates
	double normalizedX = static_cast<double>(x) / width;
	double normalizedY = static_cast<double>(y) / height;

	auto currentTime = std::chrono::high_resolution_clock::now();
	double timeInSeconds = std::chrono::duration<double>(currentTime.time_since_epoch()).count();
	double timeFactor = 0.5 * (1.0 + sin(timeInSeconds)); // Assuming currentTime is a variable representing time

// Interpolate colors based on time
	COLORREF startColor = HSLtoRGB(360.0 * timeFactor, 1.0, 0.5);  // Full hue range (0-360)
	COLORREF endColor = HSLtoRGB(360.0 * (timeFactor + 0.1), 1.0, 0.5);  // Adjust the factor for different end color

	int r = static_cast<int>((1.0 - normalizedX) * GetRValue(startColor) + normalizedX * GetRValue(endColor));
	int g = static_cast<int>((1.0 - normalizedY) * GetGValue(startColor) + normalizedY * GetGValue(endColor));
	int b = static_cast<int>((1.0 - normalizedX) * GetBValue(startColor) + normalizedX * GetBValue(endColor));

	return RGB(r, g, b);
}
void Thread::ChangeGradientColor(HWND hwnd) {
	RECT rect;
	GetClientRect(hwnd, &rect);

	// Create a memory DC and bitmap
	HDC memDC = CreateCompatibleDC(GetDC(hwnd));
	HBITMAP hBitmap = CreateCompatibleBitmap(GetDC(hwnd), rect.right, rect.bottom);
	SelectObject(memDC, hBitmap);

	// Update the background color for each pixel based on its coordinates
	for (int x = 0; x < rect.right; ++x) {
		for (int y = 0; y < rect.bottom; ++y) {
			COLORREF currentColor = GetGradientColor(x, y, rect.right, rect.bottom);
			SetPixel(memDC, x, y, currentColor);
		}
	}


	// Draw the bitmap to the screen
	BitBlt(GetDC(hwnd), 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY);

	// Clean up resources
	DeleteObject(hBitmap);
	DeleteDC(memDC);
}

DWORD WINAPI Thread::ThreadFunctionBackground(LPVOID lpParam)
{
	params = reinterpret_cast<ThreadParams*>(lpParam);
	HWND hWnd = params->hWnd;
	while (isThreadActive)
	{
		EnterCriticalSection(&cs); // Enter critical section
		ChangeGradientColor(hWnd);
		LeaveCriticalSection(&cs); // Leave critical section

		// Add sleep or other synchronization logic if needed
		Sleep(16); // Adjust sleep time as needed for desired frame rate
	}
	return 0;
}

DWORD WINAPI Thread::ThreadFunctionCircle(LPVOID lpParam)
{
	Drawer Drawer;
	params = reinterpret_cast<ThreadParams*>(lpParam);
	HWND hWnd = params->hWnd;
	MyWinApp* pApp = params->pApp;
	int N = params->N;
	int* p = params->p;
	int* q = params->q;
	SharedData* pData = params->pData;
	while (isThreadActive)
	{
		EnterCriticalSection(&cs); // Enter critical section
		HDC hdc = GetDC(hWnd);

		SelectObject(hdc, pApp->GetHpen());

		Drawer.DrawLine(hdc, pApp->GetCellWidth(), pApp->GetCellHeight());
		Drawer.DrawGrid(hdc, N, pApp->GetCellWidth(), pApp->GetCellHeight(), p, q);

		ReleaseDC(hWnd, hdc);
		LeaveCriticalSection(&cs); // Leave critical section

		// Add sleep or other synchronization logic if needed
		Sleep(16); // Adjust sleep time as needed for desired frame rate
	}
	return 0;
}

ThreadParams* Thread::InitializationThreadParams(HWND hWnd, MyWinApp* pApp, ConfigManager& configManager, int* p, int* q, SharedData* pData)
{
	params = new ThreadParams;
	params->hWnd = hWnd;
	params->pApp = pApp;
	params->N = configManager.getConfig()->getN();
	params->p = p;
	params->q = q;
	params->pData = pData;
	return params;
}
void Thread::StartThread()
{
	if (!isThreadActive)
	{
		InitializeCriticalSection(&cs); // Initialize the critical section

		// Start the thread for drawing the background
		hThreadBackground = CreateThread(NULL, 0, ThreadFunctionBackground, params, 0, NULL);
		// Start the thread for drawing the circle
		hThreadForeground = CreateThread(NULL, 0, ThreadFunctionCircle, params, 0, NULL);

		if (hThreadBackground && hThreadForeground)
		{
			isThreadActive = true;
		}
		else
		{
			// Handle thread creation failure
			// You may want to log an error or take appropriate action
			DeleteCriticalSection(&cs); // Cleanup critical section

			if (hThreadBackground)
			{
				TerminateThread(hThreadBackground, 0);
				CloseHandle(hThreadBackground);
			}

			if (hThreadForeground)
			{
				TerminateThread(hThreadForeground, 0);
				CloseHandle(hThreadForeground);
			}

			delete params;
		}
	}
}
void Thread::ResumeThreadExecution()
{
	ResumeThread(hThreadBackground);
	ResumeThread(hThreadForeground);
}

void Thread::SuspendThreadExecution()
{
	SuspendThread(hThreadBackground);
	SuspendThread(hThreadForeground);
}
void Thread::StopThread()
{
	if (isThreadActive)
	{
		if (isThreadPaused) ResumeThreadExecution();
		isThreadActive = false;

		// Wait for both threads to finish
		WaitForSingleObject(hThreadBackground, INFINITE);
		WaitForSingleObject(hThreadForeground, INFINITE);

		// Cleanup
		DeleteCriticalSection(&cs); // Cleanup critical section

		if (hThreadBackground)
		{
			TerminateThread(hThreadBackground, 0);
			CloseHandle(hThreadBackground);
		}

		if (hThreadForeground)
		{
			TerminateThread(hThreadForeground, 0);
			CloseHandle(hThreadForeground);
		}

		delete params;
	}
}


void Thread::setThreadPriority(int priority)
{
	if (hThreadBackground)
	{
		int p = GetThreadPriority(hThreadBackground);

		if (SetThreadPriority(hThreadBackground, priority))
		{
			currentPriority = priority;
			// Логика обработки успешной установки приоритета
		}
	}
	//функция нужна, чтобы отследить установился ли приоритет 
}