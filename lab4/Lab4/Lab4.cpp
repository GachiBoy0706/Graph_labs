#include "framework.h"
#include "Lab4.h"
#include "RenderClass.h"
#include <dxgi.h>
#include <d3d11.h>
#include <string>
#include <windowsx.h>


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#define MAX_LOADSTRING 100

// Глобальные переменные
WCHAR windowTitle[MAX_LOADSTRING] = L"Special Lab4 Sazhin Daniil";
WCHAR windowClassName[MAX_LOADSTRING] = L"AdvancedLab4Class";

RenderClass* g_Render = nullptr;

// Переменные для отслеживания мыши
int mouseX = 0;
int mouseY = 0;
bool mouseLeftButtonPressed = false;
bool mouseRightButtonPressed = false;

ATOM RegisterWindowClass(HINSTANCE instanceHandle);
BOOL InitializeApplication(HINSTANCE instanceHandle, int showCommand);
LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);
void HandleWindowResize(HWND windowHandle);

int APIENTRY wWinMain(_In_ HINSTANCE instanceHandle,
    _In_opt_ HINSTANCE previousInstanceHandle,
    _In_ LPWSTR commandLine,
    _In_ int showCommand)
{
    UNREFERENCED_PARAMETER(previousInstanceHandle);
    UNREFERENCED_PARAMETER(commandLine);

    if (!RegisterWindowClass(instanceHandle))
    {
        OutputDebugString(_T("Не удалось зарегистрировать класс окна\n"));
        return FALSE;
    }

    if (!InitializeApplication(instanceHandle, showCommand))
    {
        OutputDebugString(_T("Не удалось инициализировать приложение\n"));
        return FALSE;
    }

    MSG message = {};
    while (message.message != WM_QUIT)
    {
        if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        else
        {
            if (g_Render)
                g_Render->Render();
        }
    }

    if (g_Render)
    {
        g_Render->Terminate();
        delete g_Render;
        g_Render = nullptr;
    }

    return (int)message.wParam;
}

ATOM RegisterWindowClass(HINSTANCE instanceHandle)
{
    WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = instanceHandle;
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    windowClass.lpszClassName = windowClassName;
    windowClass.hIconSm = LoadIcon(instanceHandle, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&windowClass);
}

BOOL InitializeApplication(HINSTANCE instanceHandle, int showCommand)
{
    HWND windowHandle = CreateWindowW(windowClassName, windowTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
        nullptr, nullptr, instanceHandle, nullptr);
    if (!windowHandle)
    {
        OutputDebugString(_T("Не удалось создать окно\n"));
        return FALSE;
    }

    g_Render = new RenderClass();
    if (FAILED(g_Render->Init(windowHandle, windowTitle, windowClassName)))
    {
        OutputDebugString(_T("Не удалось инициализировать рендерер\n"));
        delete g_Render;
        g_Render = nullptr;
        return FALSE;
    }

    ShowWindow(windowHandle, showCommand);
    UpdateWindow(windowHandle);

    return TRUE;
}

LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SIZE:
        HandleWindowResize(windowHandle);
        break;

    case WM_MOUSEMOVE:
        if (mouseLeftButtonPressed || mouseRightButtonPressed)
        {
            int newX = GET_X_LPARAM(lParam);
            int newY = GET_Y_LPARAM(lParam);

            if (mouseLeftButtonPressed)
            {
                // Вращение камеры при движении мыши с левой кнопкой
                g_Render->RotateCamera((newX - mouseX) * 0.01f, (newY - mouseY) * 0.01f);
            }
            else if (mouseRightButtonPressed)
            {
                // Движение камеры при движении мыши с правой кнопкой
                g_Render->MoveCamera((newX - mouseX) * 0.01f, -(newY - mouseY) * 0.01f, 0.0f);
            }

            mouseX = newX;
            mouseY = newY;
        }
        break;

    case WM_LBUTTONDOWN:
        mouseLeftButtonPressed = true;
        mouseX = GET_X_LPARAM(lParam);
        mouseY = GET_Y_LPARAM(lParam);
        break;

    case WM_RBUTTONDOWN:
        mouseRightButtonPressed = true;
        mouseX = GET_X_LPARAM(lParam);
        mouseY = GET_Y_LPARAM(lParam);
        break;

    case WM_LBUTTONUP:
        mouseLeftButtonPressed = false;
        break;

    case WM_RBUTTONUP:
        mouseRightButtonPressed = false;
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(windowHandle, message, wParam, lParam);
    }
    return 0;
}

void HandleWindowResize(HWND windowHandle)
{
    if (g_Render)
        g_Render->Resize(windowHandle);
}
