#include "WindowsProject1.h"
#include "Config.h"
#include "Drawer.h"
#include "Thread.h"
#include "EventHandler.h"
#include <process.h>

HPEN MyWinApp::hpen = 0;
HBRUSH MyWinApp::Brush = 0;
int MyWinApp::cellWidth = 0;
int MyWinApp::cellHeight = 0;
PAINTSTRUCT MyWinApp::ps = {};
ConfigManager MyWinApp::configManager = {};

UINT WM_PRESSED;
UINT WM_SEMAPHORE_RELEASED;

HANDLE SemaphoreNumberPlayers;
DWORD ResultSemaphoreNumberPlayers;
HANDLE Event;
std::mutex MutexMovePlayers;

SharedData* pData;
// Функция для установки номера игрока для окна
void SetPlayerNumberForWindow(DWORD ProcessId, int PlayerNumber) {
	for (int i = 0; i < NUNBER_OF_PLAYERS; ++i) {
		if (pData->infoPlayers[i].ProcessId == 0) {
			pData->infoPlayers[i].ProcessId = ProcessId;
			pData->infoPlayers[i].PlayerNumber = PlayerNumber;
			break;
		}
	}
}

// Функция для получения номера игрока для окна
int GetPlayerNumberForWindow(DWORD ProcessId) {
	for (int i = 0; i < NUNBER_OF_PLAYERS; ++i) {
		if (pData->infoPlayers[i].ProcessId == ProcessId) {
			return pData->infoPlayers[i].PlayerNumber;
		}
	}
	return -1; // Вернуть -1, если не найдено соответствие
}

MyWinApp::MyWinApp(HINSTANCE hInstance) : hInstance(hInstance), hWnd(nullptr) {
	// Register the window class
	WM_PRESSED = RegisterWindowMessage(TEXT("MyApp_PressedMsg"));
	WM_SEMAPHORE_RELEASED = RegisterWindowMessage(TEXT("SemaphoreReleased"));
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
	hWnd = CreateWindow(L"MyWinApp", L"Наблюдатель", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, configManager.getConfig()->getHeight(), configManager.getConfig()->getHeight(), nullptr, nullptr, hInstance, this);

	if (!hWnd) {
		// Handle window creation error
		return;
	}

	Event = CreateEvent(NULL, FALSE, TRUE, L"MyEvent");

	if (WaitForSingleObject(Event, INFINITE) == WAIT_OBJECT_0) {
		if (ResultSemaphoreNumberPlayers == WAIT_OBJECT_0) {
			if (pData->currentPlayer == 0) {
				int result = MessageBox(hWnd, L"Желаете играть за 'X'?", L"Выбор команды", MB_YESNO);

				if (result == IDYES) {
					SetWindowText(hWnd, L"Крестики");
					pData->setPlayer = 0;
				}
				else {
					SetWindowText(hWnd, L"Нолики");
					pData->setPlayer = 1;
				}
				pData->currentPlayer++;
			}
			else {
				if (pData->setPlayer == 1) {
					SetWindowText(hWnd, L"Крестики");
					pData->setPlayer = 0;
				}

				else {
					SetWindowText(hWnd, L"Нолики");
					pData->setPlayer = 1;
				}
				pData->currentPlayer--;
			}

			SetPlayerNumberForWindow(GetCurrentProcessId(), pData->setPlayer);

		}
		SetEvent(Event);
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

void MyWinApp::GettingData(HWND hWnd, UINT message, MyWinApp** pApp, LPARAM lParam) {
	if (message == WM_NCCREATE) {
		*pApp = static_cast<MyWinApp*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pApp));
	}
	else {
		*pApp = reinterpret_cast<MyWinApp*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}
}

void CleanResource(HWND hWnd, HBRUSH Brush, HPEN* hpen, int* p, HANDLE hFileMemory, ConfigManager& configManager, Thread thread) {
	UnregisterHotKey(hWnd, 0);
	UnregisterHotKey(hWnd, 1);

	DeleteObject(Brush);
	DeleteObject(hpen);

	if (p != NULL) UnmapViewOfFile(p);
	if (hFileMemory != NULL) CloseHandle(hFileMemory);
	if (ResultSemaphoreNumberPlayers == WAIT_OBJECT_0)ReleaseSemaphore(SemaphoreNumberPlayers, 1, NULL);
	CloseHandle(SemaphoreNumberPlayers);
	CloseHandle(Event);
	if (thread.isThreadActive) thread.StopThread();

	configManager.save();
}

void ProcessingMOUSEWHEEL(HWND hWnd, HPEN* hpen, WPARAM wParam, EventHandler eventHandler, ConfigManager& configManager) {
	COLORREF color;
	color = configManager.getConfig()->getGridColor();

	eventHandler.HandleMouseWheel(wParam, color);
	configManager.getConfig()->setGridColor(color);
	DeleteObject(hpen);

	*hpen = CreatePen(PS_SOLID, PEN_WIDTH, configManager.getConfig()->getGridColor());
	InvalidateRect(hWnd, NULL, TRUE);
}


void ProcessingRETURN(HWND hWnd, HBRUSH Brush, ConfigManager& configManager) {
	COLORREF color;
	color = RGB(rand() % 256, rand() % 256, rand() % 256);

	configManager.getConfig()->setBackgroundColor(color);
	DeleteObject(Brush);

	Brush = CreateSolidBrush(configManager.getConfig()->getBackgroundColor());
	SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)Brush);
	InvalidateRect(hWnd, NULL, TRUE);
}

void computeCell(LPARAM lParam, ConfigManager& configManager, int& cellWidth, int& cellHeight)
{
	cellWidth = LOWORD(lParam) / configManager.getConfig()->getN();
	cellHeight = HIWORD(lParam) / configManager.getConfig()->getN();
}

void registerHotKeys(HWND hWnd)
{
	RegisterHotKey(hWnd, 0, MOD_CONTROL, 'Q');
	RegisterHotKey(hWnd, 1, MOD_SHIFT, 'C');
}
bool CheckingLocationSigns(int* p, int* q, int symbol, ConfigManager& configManager) {
	q = p;
	// Проверка горизонталей и вертикалей
	for (int i = 0; i < configManager.getConfig()->getN(); ++i) {
		bool winRow = true, winCol = true;

		for (int j = 0; j < configManager.getConfig()->getN(); ++j) {
			// Проверка горизонтали
			int indexRow = i * configManager.getConfig()->getN() + j;
			if (q[indexRow] != symbol) {
				winRow = false;
			}

			// Проверка вертикали
			int indexCol = j * configManager.getConfig()->getN() + i;
			if (q[indexCol] != symbol) {
				winCol = false;
			}
		}

		if (winRow || winCol) {
			return true;
		}
	}

	// Проверка диагоналей
	bool winDiagonal1 = true, winDiagonal2 = true;
	for (int i = 0; i < configManager.getConfig()->getN(); ++i) {
		int indexDiagonal1 = i * configManager.getConfig()->getN() + i;
		int indexDiagonal2 = i * configManager.getConfig()->getN() + (configManager.getConfig()->getN() - 1 - i);

		if (q[indexDiagonal1] != symbol) {
			winDiagonal1 = false;
		}

		if (q[indexDiagonal2] != symbol) {
			winDiagonal2 = false;
		}
	}
	if (winDiagonal1 || winDiagonal2) {
		return true;
	}

	return false;
}
bool CheckingDraw(int* p, int* q, ConfigManager& configManager) {
	q = p;
	for (int i = 0; i < configManager.getConfig()->getN(); ++i) {
		for (int j = 0; j < configManager.getConfig()->getN(); ++j) {
			int index = i * configManager.getConfig()->getN() + j;
			if (q[index] == 0) {
				return false;
			}
		}
	}
	return true;
}
void CheckingWinners(HWND hWnd, HBRUSH Brush, HPEN* hpen, int* p, int* q, HANDLE hFileMemory, ConfigManager& configManager, Thread thread)
{

	if (CheckingLocationSigns(p, q, 1, configManager)) {
		MessageBox(hWnd, _T("Игрок с символом 'X' выиграл!"), _T("Игра окончена"), MB_OK | MB_ICONQUESTION);
		CleanResource(hWnd, Brush, hpen, p, hFileMemory, configManager, thread);
		PostQuitMessage(0);
	}

	if (CheckingLocationSigns(p, q, 2, configManager)) {
		MessageBox(hWnd, _T("Игрок с символом 'O' выиграл!"), _T("Игра окончена"), MB_OK | MB_ICONQUESTION);
		CleanResource(hWnd, Brush, hpen, p, hFileMemory, configManager, thread);
		PostQuitMessage(0);
	}

	if (CheckingDraw(p, q, configManager)) {
		MessageBox(hWnd, _T("Ничья!"), _T("Игра окончена"), MB_OK | MB_ICONQUESTION);
		CleanResource(hWnd, Brush, hpen, p, hFileMemory, configManager, thread);
		PostQuitMessage(0);
	}


}
void InitializeMemory(int* p, int* q, ConfigManager& configManager)
{
	int i, j;
	for (i = 0, q = p; i < configManager.getConfig()->getN(); i++)
		for (j = 0; j < configManager.getConfig()->getN(); j++, q++)
			*q = 0;
}
void UpdatingField(int* p, int* q, ConfigManager& configManager) {
	int k, m;
	for (k = 0; k < configManager.getConfig()->getN(); k++)
		for (m = 0; m < configManager.getConfig()->getN(); m++, p++) {

			*p = pData->moveHistory[k][m].value;
		}
}
HANDLE GetSharedMemory(ConfigManager& configManager) {
	HANDLE hSharedMemory = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHARED_FILE_NAME);
	if (hSharedMemory == NULL) {
		hSharedMemory = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SharedData), SHARED_FILE_NAME);
	}
	return hSharedMemory;
}
LRESULT CALLBACK MyWinApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	MyWinApp* pApp = 0;
	EventHandler eventHandler;
	Drawer Drawer;
	Thread thread;
	static int* p = new int[configManager.getConfig()->getN() * configManager.getConfig()->getN()];
	static int* q = 0;
	static HANDLE hFileMemory;
	static LPVOID mappedMemory;
	SIZE_T dataSize = sizeof(SharedData);

	GettingData(hWnd, message, &pApp, lParam);

	if (pApp) {
		switch (message) {
		case WM_SIZE: {Drawer.SetSize(LOWORD(lParam), HIWORD(lParam)); }
		case WM_CREATE:
		{

			hpen = CreatePen(PS_SOLID, PEN_WIDTH, configManager.getConfig()->getGridColor());

			registerHotKeys(hWnd);
			computeCell(lParam, configManager, cellWidth, cellHeight);

			hFileMemory = GetSharedMemory(configManager);
			if (hFileMemory == NULL) {
				DestroyWindow(hWnd);
				return 0;
			}

			mappedMemory = MapViewOfFile(hFileMemory, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedData));
			if (mappedMemory == NULL) {
				CloseHandle(hFileMemory);
				DestroyWindow(hWnd);
				return 0;
			}

			pData = reinterpret_cast<SharedData*>(mappedMemory);

			bool FirstInstance = (GetLastError() != ERROR_ALREADY_EXISTS);
			if (FirstInstance) {

				InitializeMemory(p, q, configManager);
			}

		}
		break;
		case WM_KEYDOWN:
		{
			switch (LOWORD(wParam))
			{
			case VK_ESCAPE: {DestroyWindow(hWnd); break; }
			case VK_RETURN: {ProcessingRETURN(hWnd, Brush, configManager); break; }
			case VK_SPACE: {
				if (!thread.isThreadActive)
				{
					thread.InitializationThreadParams(hWnd, pApp, configManager, p, q, pData);
					thread.StartThread();
				}
				else
				{
					thread.StopThread();
					InvalidateRect(hWnd, NULL, TRUE);
				}
				break;
			}
			case VK_SHIFT: {
				if (thread.isThreadActive)
				{
					if (thread.isThreadPaused)
					{
						thread.ResumeThreadExecution();
					}
					else
					{
						thread.SuspendThreadExecution();
					}
					thread.isThreadPaused = !thread.isThreadPaused;
				}
				break;
			}
			case '1': {
				//Самый высокий приоритет в реальном времени.
				Thread::setThreadPriority(THREAD_PRIORITY_TIME_CRITICAL);
				break;
			}
			case '2': {
				//Приоритет выше обычного.
				Thread::setThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
				break;
			}
			case '3': {
				//Обычный приоритет, используемый по умолчанию.
				Thread::setThreadPriority(THREAD_PRIORITY_NORMAL);
				break;
			}
			case '4': {
				//Фоновый приоритет
				Thread::setThreadPriority(THREAD_PRIORITY_BELOW_NORMAL);
				break;
			}
			case '5': {
				//Низший приоритет.
				Thread::setThreadPriority(THREAD_PRIORITY_LOWEST);
				break;
			}
			case '6': {
				//Самый низкий приоритет, используемый для потоков, выполняющих задачи в режиме ожидания.
				Thread::setThreadPriority(THREAD_PRIORITY_IDLE);
				break;
			}
			default: break;
			}
		}
		break;
		case WM_HOTKEY: {eventHandler.HandleHotKey(hWnd, wParam); break; }
		case WM_MOUSEWHEEL: {ProcessingMOUSEWHEEL(hWnd, &hpen, wParam, eventHandler, configManager); break; }


		case WM_RBUTTONDOWN: {
			if (ResultSemaphoreNumberPlayers == WAIT_OBJECT_0)
			{
				std::unique_lock<std::mutex> lock(MutexMovePlayers);
				if (pData->currentPlayer == GetPlayerNumberForWindow(GetCurrentProcessId()) && pData->currentPlayer == 0) {
				eventHandler.HandleRightMouseClick(hWnd, lParam, cellWidth, cellHeight, configManager.getConfig()->getN(), p, q, pData);
				pData->currentPlayer = (pData->currentPlayer + 1) % NUNBER_OF_PLAYERS;
				// Отправить сообщение всем окнам.
				SendMessage(HWND_BROADCAST, WM_PRESSED, 0, 0);
				InvalidateRect(hWnd, NULL, TRUE);
				}
			}
			else
			{
				// Если счетчик семафора равен 0, значит уже запущено максимальное количество игр
				MessageBox(NULL, L"Достигнуто максимальное количество игр. Вы можете только наблюдать за игрой.", L"Ошибка", MB_OK | MB_ICONERROR);
			}
			break;
		}

		case WM_LBUTTONDOWN: {
			if (ResultSemaphoreNumberPlayers == WAIT_OBJECT_0)
			{
				std::unique_lock<std::mutex> lock(MutexMovePlayers);
				if (pData->currentPlayer == GetPlayerNumberForWindow(GetCurrentProcessId()) && pData->currentPlayer == 1) {
					eventHandler.HandleLeftMouseClick(hWnd, lParam, cellWidth, cellHeight, configManager.getConfig()->getN(), p, q, pData);
					pData->currentPlayer = (pData->currentPlayer + 1) % NUNBER_OF_PLAYERS;
					// Отправить сообщение всем окнам.
					SendMessage(HWND_BROADCAST, WM_PRESSED, 0, 0);
					InvalidateRect(hWnd, NULL, TRUE);
				}
			}
			else
			{
				// Если счетчик семафора равен 0, значит уже запущено максимальное количество игр
				MessageBox(NULL, L"Достигнуто максимальное количество игр. Вы можете только наблюдать за игрой.", L"Ошибка", MB_OK | MB_ICONERROR);
			}
			break;
		}

		case WM_PAINT:
		{
			HDC hdc = BeginPaint(hWnd, &ps);
			SelectObject(hdc, hpen);
			UpdatingField(p, q, configManager);
			Drawer.DrawLine(hdc, cellWidth, cellHeight);
			Drawer.DrawGrid(hdc, configManager.getConfig()->getN(), cellWidth, cellHeight, p, q);
			DeleteDC(hdc);

			EndPaint(hWnd, &ps);
			CheckingWinners(hWnd, Brush, &hpen, p, q, hFileMemory, configManager, thread);
		}
		break;
		case WM_DESTROY: {
			CleanResource(hWnd, Brush, &hpen, p, hFileMemory, configManager, thread);
			PostQuitMessage(0);
			SendMessage(HWND_BROADCAST, WM_SEMAPHORE_RELEASED, 0, 0); }
		default:
			if (message == WM_PRESSED) {
				InvalidateRect(hWnd, NULL, TRUE);
			}
			else if (message == WM_SEMAPHORE_RELEASED && ResultSemaphoreNumberPlayers != WAIT_OBJECT_0) {
				ResultSemaphoreNumberPlayers = WaitForSingleObject(SemaphoreNumberPlayers, 0);
			}
			else {
				return DefWindowProc(hWnd, message, wParam, lParam);
			}

		}
	}
	else return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	SemaphoreNumberPlayers = CreateSemaphore(NULL, 2, 2, L"Global\\MySemaphore");
	ResultSemaphoreNumberPlayers = WaitForSingleObject(SemaphoreNumberPlayers, 0);
	MyWinApp myApp(hInstance);

	return myApp.Run();
}