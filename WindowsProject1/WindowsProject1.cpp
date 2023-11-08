// WindowsProject4.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "WindowsProject1.h"
#include "Config.h"
#define MAX_LOADSTRING 100
int WIDTH;
int HEIGHT;
int N;
COLORREF gridColor;
COLORREF backgroundColor;

const char EMPTY = ' ';
const char CROSS = 'X';
const char NOLL = 'O';



std::vector<std::vector<char>> grid;
// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    // Разбор параметров командной строки
    init(&WIDTH,
        &HEIGHT,
        &N,
        &gridColor,
        &backgroundColor);

    if (__argc >= 2)
    {
        N = atoi(__argv[1]);
    }

    grid.resize(N, std::vector<char>(N, EMPTY));
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(backgroundColor);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

void ColorMouse(HWND hWnd, WPARAM wParam)
{
    int r = GetRValue(gridColor);
    int g = GetGValue(gridColor);
    int b = GetBValue(gridColor);
    int difference = GET_WHEEL_DELTA_WPARAM(wParam);

    // Изменение цвета фона
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

void DrawEllipse(HDC hdc, int x, int y, int cellWidth, int cellHeight)
{
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 250, 0));
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 255));

    SelectObject(hdc, hPen);
    SelectObject(hdc, hBrush);

    Ellipse(hdc, x - cellWidth / 2, y - cellHeight / 2, x + cellWidth / 2, y + cellHeight / 2);

    DeleteObject(hPen);
    DeleteObject(hBrush);
}

void DrawCross(HDC hdc, int x, int y, int cellWidth, int cellHeight)
{
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 250, 255));
    SelectObject(hdc, hPen);

    MoveToEx(hdc, x, y, NULL);
    LineTo(hdc, x + cellWidth, y + cellHeight);

    MoveToEx(hdc, x, y + cellHeight, NULL);
    LineTo(hdc, x + cellWidth, y);

    DeleteObject(hPen);
}

void DrawLine(HDC hdc, int x, int y, int sx, int sy, int cellWidth, int cellHeight)
{
    for (int x = 0; x <= sx; x += cellWidth)
    {
        MoveToEx(hdc, x, 0, NULL);
        LineTo(hdc, x, sy);

    }

    for (int y = 0; y <= sy; y += cellHeight)
    {
        MoveToEx(hdc, 0, y, NULL);
        LineTo(hdc, sx, y);
    }
}
//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    STARTUPINFO tin;
    PROCESS_INFORMATION pInfo;
    DWORD exitCode;
    TCHAR CommandLine[256] = _T("notepad");
    PAINTSTRUCT ps;
    static int sx, sy, x, y;
    static HPEN hpen;
    static HBRUSH Brush;
    int cellWidth = sx / N;
    int cellHeight = sy / N;
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Разобрать выбор в меню:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_CREATE:
    {
        hpen = CreatePen(PS_SOLID, 5, gridColor);

        RegisterHotKey(hWnd, 0, MOD_CONTROL, 'Q');
        RegisterHotKey(hWnd, 1, MOD_SHIFT, 'C');

        tin.cb = sizeof(STARTUPINFO);
        tin.dwFlags = STARTF_USESHOWWINDOW;
        tin.wShowWindow = SW_SHOWNORMAL;

    }
    break;
    case WM_SIZE:
    {
        sx = LOWORD(lParam);
        sy = HIWORD(lParam);
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
            backgroundColor = RGB(rand() % 256, rand() % 256, rand() % 256);
            Brush = CreateSolidBrush(backgroundColor);
            SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)Brush);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;
        default:
            break;
        }
    }
    case WM_HOTKEY:
    {
        if (wParam == 0)   DestroyWindow(hWnd);
        if (wParam == 1)
            if (CreateProcess(NULL, CommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &tin, &pInfo)) {
                CloseHandle(pInfo.hProcess);
                CloseHandle(pInfo.hThread);
            }
            else {
                MessageBox(NULL, L"Не удалось запустить блокнот.", L"Ошибка", MB_OK | MB_ICONERROR);
            }
    }
    break;
    case WM_MOUSEWHEEL:
    {
        ColorMouse(hWnd, wParam);
        // Уничтожаем старое перо и создаем новое с обновленным цветом
        DeleteObject(hpen);
        hpen = CreatePen(PS_SOLID, 5, gridColor);
        // Заставляем окно перерисоваться
        InvalidateRect(hWnd, NULL, TRUE);
    }
    break;
    case WM_RBUTTONDOWN: {
        x = LOWORD(lParam) / cellWidth;
        y = HIWORD(lParam) / cellHeight;

        if (x >= 0 && x < N && y >= 0 && y < N) {
            grid[x][y] = 'X';
            InvalidateRect(hWnd, NULL, TRUE); // Перерисовываем окно
        }
    }
                       break;
    case WM_LBUTTONDOWN: {
        x = LOWORD(lParam) / cellWidth;
        y = HIWORD(lParam) / cellHeight;

        if (x >= 0 && x < N && y >= 0 && y < N) {
            grid[x][y] = 'O';
            InvalidateRect(hWnd, NULL, TRUE); // Перерисовываем окно
        }
    }
                       break;
    case WM_PAINT:
    {
        HDC hdc = BeginPaint(hWnd, &ps);
        SelectObject(hdc, hpen);

        DrawLine(hdc, x, y, sx, sy, cellWidth, cellHeight);

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {

                if (grid[i][j] == 'X') {
                    x = i * cellWidth;
                    y = j * cellHeight;
                    DrawCross(hdc, x, y, cellWidth, cellHeight);
                }

                if (grid[i][j] == 'O') {
                    x = i * cellWidth + cellWidth / 2;
                    y = j * cellHeight + cellHeight / 2;
                    DrawEllipse(hdc, x, y, cellWidth, cellHeight);
                }
            }
        }

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        UnregisterHotKey(hWnd, 0);
        UnregisterHotKey(hWnd, 1);
        DeleteObject(Brush);
        DeleteObject(hpen);
        PostQuitMessage(0);
        save(&WIDTH,
            &HEIGHT,
            &N,
            &gridColor,
            &backgroundColor);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
