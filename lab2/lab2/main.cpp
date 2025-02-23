#include <windows.h>
#include <xstring>

#include "resource.h"
#include "renderer.h"

#define MAX_LOADSTRING 300
WCHAR appTitle[MAX_LOADSTRING];

HINSTANCE       appInstance = nullptr;
HWND            mainWindow = nullptr;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
UINT windowWidth = 1280, windowHeight = 720;

HRESULT InitializeWindow(HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASSEX windowClass;
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = hInstance;
    windowClass.hIcon = nullptr;
    windowClass.hIconSm = nullptr;
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    windowClass.lpszMenuName = nullptr;
    windowClass.lpszClassName = L"CustomWindowClass";

    if (!RegisterClassEx(&windowClass))
        return E_FAIL;

    appInstance = hInstance;
    RECT windowRect = { 0, 0, 1280, 720 };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
    mainWindow = CreateWindow(L"CustomWindowClass", L"Lab 2 - Sazhin Daniil",
        WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr, hInstance,
        nullptr);

    if (!mainWindow)
        return E_FAIL;

    ShowWindow(mainWindow, nCmdShow);

    return S_OK;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if (FAILED(InitializeWindow(hInstance, nCmdShow)))
        return 0;

    LoadStringW(hInstance, IDS_APP_TITLE, appTitle, MAX_LOADSTRING);

    std::wstring currentDir;
    currentDir.resize(MAX_LOADSTRING + 1);
    GetCurrentDirectory(MAX_LOADSTRING + 1, &currentDir[0]);
    size_t configPos = currentDir.find(L"x64");
    if (configPos != std::wstring::npos)
    {
        currentDir.resize(configPos);
        currentDir += appTitle;
        SetCurrentDirectory(currentDir.c_str());
    }

    if (FAILED(Renderer::getInstance().initDevice(mainWindow)))
    {
        Renderer::getInstance().deviceCleanup();
        return 0;
    }

    MSG message = { 0 };
    while (WM_QUIT != message.message)
    {
        if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        Renderer::getInstance().render();
    }

    Renderer::getInstance().deviceCleanup();

    return (int)message.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT paintStruct;
    HDC deviceContext;

    switch (message)
    {
    case WM_PAINT:
        deviceContext = BeginPaint(hWnd, &paintStruct);
        EndPaint(hWnd, &paintStruct);
        break;

    case WM_GETMINMAXINFO:
    {
        LPMINMAXINFO minMaxInfo = (LPMINMAXINFO)lParam;
        minMaxInfo->ptMinTrackSize.x = 256;
        minMaxInfo->ptMinTrackSize.y = 256;
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_SIZE:
        Renderer::getInstance().resizeWindow(mainWindow);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}
