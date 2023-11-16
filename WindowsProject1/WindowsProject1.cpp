#include "WindowsProject1.h"
#include "Config.h"
#include "Drawing.h"
#include "EventHandling.h"

int MyWinApp::sx = 0;
int MyWinApp::sy = 0;
int MyWinApp::x = 0;
int MyWinApp::y = 0;
HPEN MyWinApp::hpen = 0;
HBRUSH MyWinApp::Brush = 0;
STARTUPINFO MyWinApp::tin = {};
PROCESS_INFORMATION MyWinApp::pInfo = {};
int MyWinApp::cellWidth = 0;
int MyWinApp::cellHeight = 0;
PAINTSTRUCT MyWinApp::ps = {};
static Config configManager;
int flag = 0;
MyWinApp::MyWinApp(HINSTANCE hInstance) : hInstance(hInstance), hWnd(nullptr) {
	// Register the window class

	if (__argc >= 3)
	{
		if (strcmp(__argv[2], "1") == 0) {
			configManager.initMap();
			flag = 1;
			MessageBox(hWnd, _T("Map"), _T("Map"),
				MB_OK | MB_ICONQUESTION);
		}
		if (strcmp(__argv[2], "2") == 0) {
			configManager.initFileVariable();
			flag = 2;
			MessageBox(hWnd, _T("FileVariable"), _T("FileVariable"),
				MB_OK | MB_ICONQUESTION);
		}
		if (strcmp(__argv[2], "3") == 0) {
			configManager.init();
			flag = 3;
			MessageBox(hWnd, _T("Stream"), _T("Stream"),
				MB_OK | MB_ICONQUESTION);
		}
		if (strcmp(__argv[2], "4") == 0) {
			configManager.initWinAPI();
			flag = 4;
			MessageBox(hWnd, _T("WinAPI"), _T("WinAPI"),
				MB_OK | MB_ICONQUESTION);
		}
	}

	else configManager.init();

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(configManager.backgroundColor);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"MyWinApp";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex)) {
		// Handle class registration error
		return;
	}

	// Create the window
	hWnd = CreateWindow(L"MyWinApp", L"My WinAPI App", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, configManager.WIDTH, configManager.HEIGHT, nullptr, nullptr, hInstance, this);

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
	static std::vector<std::vector<char>> grid;

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
			sx = LOWORD(lParam);
			sy = HIWORD(lParam);
		}
		case WM_CREATE:
		{
			if (__argc >= 2)
			{
				configManager.N = atoi(__argv[1]);
			}

			hpen = CreatePen(PS_SOLID, 5, configManager.gridColor);

			RegisterHotKey(hWnd, 0, MOD_CONTROL, 'Q');
			RegisterHotKey(hWnd, 1, MOD_SHIFT, 'C');

			cellWidth = LOWORD(lParam) / configManager.N;
			cellHeight = HIWORD(lParam) / configManager.N;
			grid.resize(configManager.N, std::vector<char>(configManager.N, ' '));
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
				configManager.backgroundColor = RGB(rand() % 256, rand() % 256, rand() % 256);
				DeleteObject(Brush);
				Brush = CreateSolidBrush(configManager.backgroundColor);
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
			EventHandling::HandleHotKey(hWnd, wParam, tin, pInfo);
		}
		break;
		case WM_MOUSEWHEEL:
		{
			EventHandling::HandleMouseWheel(hWnd, wParam, configManager.gridColor);
			DeleteObject(hpen);
			hpen = CreatePen(PS_SOLID, 5, configManager.gridColor);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
		case WM_RBUTTONDOWN: {
			EventHandling::HandleRightMouseClick(hWnd, lParam, cellWidth, cellHeight, configManager.N, grid);
		}
						   break;
		case WM_LBUTTONDOWN: {
			EventHandling::HandleLeftMouseClick(hWnd, lParam, cellWidth, cellHeight, configManager.N, grid);
		}
						   break;
		case WM_PAINT:
		{
			HDC hdc = BeginPaint(hWnd, &ps);
			SelectObject(hdc, hpen);

			Drawing::DrawLine(hdc, x, y, sx, sy, cellWidth, cellHeight);
			Drawing::DrawGrid(hdc, configManager.N, cellWidth, cellHeight, grid);

			EndPaint(hWnd, &ps);
		}
		break;
		case WM_DESTROY:
			UnregisterHotKey(hWnd, 0);
			UnregisterHotKey(hWnd, 1);
			DeleteObject(Brush);
			DeleteObject(hpen);
			PostQuitMessage(0);

			if (flag == 1) {
				configManager.saveMap();
			}
			if (flag == 2) {
				configManager.saveFileVariable();
			}
			if (flag == 3) {
				configManager.save();
			}
			if (flag == 4) {
				configManager.saveWinAPI();
			}
			else configManager.save();
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
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