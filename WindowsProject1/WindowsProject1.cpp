#include "WindowsProject1.h"
#include "Config.h"
#include "Drawing.h"
#include "EventHandling.h"
HPEN MyWinApp::hpen = 0;
HBRUSH MyWinApp::Brush = 0;

int MyWinApp::cellWidth = 0;
int MyWinApp::cellHeight = 0;
PAINTSTRUCT MyWinApp::ps = {};
ConfigManager MyWinApp::configManager = {};

UINT WM_PRESSED;
MyWinApp::MyWinApp(HINSTANCE hInstance) : hInstance(hInstance), hWnd(nullptr) {
	// Register the window class
	WM_PRESSED = RegisterWindowMessage(TEXT("MyApp_PressedMsg"));
	Factory::setConfigInit(hWnd, __argv[1], configManager);
	configManager.init();

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(configManager.getConfig()->getBackgroundColor());
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"MyWinApp";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex)) {
		// Handle class registration error
		return;
	}

	// Create the window
	hWnd = CreateWindow(L"MyWinApp", L"My WinAPI App", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, configManager.getConfig()->getHeight(), configManager.getConfig()->getHeight(), nullptr, nullptr, hInstance, this);

	if (!hWnd) {
		// Handle window creation error
		return;
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
}

MyWinApp::~MyWinApp() {
	// Perform cleanup here if needed
}

int MyWinApp::Run() {
	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK MyWinApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	MyWinApp* pApp;
	COLORREF color;
	EventHandling eventHandler;
	Drawing drawing;
	static int* p = 0;
	static int* q = 0;
	int i, j;
	TCHAR str[10];
	static HANDLE hFileMemory;
	if (message == WM_NCCREATE) {
		pApp = static_cast<MyWinApp*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pApp));
	}
	else {
		pApp = reinterpret_cast<MyWinApp*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	if (pApp) {
		switch (message) {
		case WM_SIZE:
		{
			drawing.SetSize(LOWORD(lParam), HIWORD(lParam));

		}
		case WM_CREATE:
		{
			//if (__argc >= 2)
			//{
			//	configManager.getConfig()->setN(atoi(__argv[1]));
			//}

			hpen = CreatePen(PS_SOLID, PEN_WIDTH, configManager.getConfig()->getGridColor());

			RegisterHotKey(hWnd, 0, MOD_CONTROL, 'Q');
			RegisterHotKey(hWnd, 1, MOD_SHIFT, 'C');

			cellWidth = LOWORD(lParam) / configManager.getConfig()->getN();
			cellHeight = HIWORD(lParam) / configManager.getConfig()->getN();

			hFileMemory = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SHARED_BUFFER_SIZE, _T("Shared"));
			if (hFileMemory == NULL) { 
				DestroyWindow(hWnd); break;
			}

			p = (int*)MapViewOfFile(hFileMemory, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
			if (p == NULL) {
				CloseHandle(hFileMemory);
				DestroyWindow(hWnd); break;
			}

			for (i = 0, q = p; i < configManager.getConfig()->getN(); i++)
				for (j = 0; j < configManager.getConfig()->getN(); j++, q++) *q = 0;
		}
		break;
		case WM_KEYDOWN:
		{
			switch (LOWORD(wParam))
			{
			case VK_ESCAPE:
			{
				DestroyWindow(hWnd);
			}
			break;
			case VK_RETURN:
			{
				color = RGB(rand() % 256, rand() % 256, rand() % 256);
				configManager.getConfig()->setBackgroundColor(color);
				DeleteObject(Brush);
				Brush = CreateSolidBrush(configManager.getConfig()->getBackgroundColor());
				SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)Brush);
				InvalidateRect(hWnd, NULL, TRUE);
			}
			break;
			default:
				break;
			}
		}
		break;
		case WM_HOTKEY:
		{
			eventHandler.HandleHotKey(hWnd, wParam);
		}
		break;
		case WM_MOUSEWHEEL:
		{
			color = configManager.getConfig()->getGridColor();
			eventHandler.HandleMouseWheel(wParam, color);
			configManager.getConfig()->setGridColor(color);
			DeleteObject(hpen);
			hpen = CreatePen(PS_SOLID, PEN_WIDTH, configManager.getConfig()->getGridColor());
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
		case WM_RBUTTONDOWN: {
			eventHandler.HandleRightMouseClick(hWnd, lParam, cellWidth, cellHeight, configManager.getConfig()->getN(), p, q);
			// Отправить сообщение всем окнам.
			PostMessage(HWND_BROADCAST, WM_PRESSED, 0, 0);
			InvalidateRect(hWnd, NULL, TRUE);
		}
						   break;
		case WM_LBUTTONDOWN: {
			eventHandler.HandleLeftMouseClick(hWnd, lParam, cellWidth, cellHeight, configManager.getConfig()->getN(), p, q);
			// Отправить сообщение всем окнам.
			PostMessage(HWND_BROADCAST, WM_PRESSED, 0, 0);
			InvalidateRect(hWnd, NULL, TRUE);
		}
						   break;
		case WM_PAINT:
		{
			HDC hdc = BeginPaint(hWnd, &ps);
			SelectObject(hdc, hpen);

			drawing.DrawLine(hdc, cellWidth, cellHeight);
			drawing.DrawGrid(hdc, configManager.getConfig()->getN(), cellWidth, cellHeight, p, q);
			DeleteDC(hdc);
			EndPaint(hWnd, &ps);
		}
		break;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDM_EXIT: DestroyWindow(hWnd); break;
			default: return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_DESTROY:
			UnregisterHotKey(hWnd, 0);
			UnregisterHotKey(hWnd, 1);
			DeleteObject(Brush);
			DeleteObject(hpen);
			if (p != NULL) UnmapViewOfFile(p);
			if (hFileMemory != NULL) CloseHandle(hFileMemory);
			PostQuitMessage(0);
			configManager.save();
		default:
			if (message == WM_PRESSED) {
				InvalidateRect(hWnd, NULL, TRUE);
			}
			else {
				return DefWindowProc(hWnd, message, wParam, lParam);
			}

		}
	}
	else {
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	MyWinApp myApp(hInstance);

	return myApp.Run();
}