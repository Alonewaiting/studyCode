#include <windows.h>
#include <exception>
#include <iostream>

#include "D3D11RenderTexture.h"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
constexpr size_t width =1200;
constexpr size_t height = 1800;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR szCmdLine, int iCmdShow)
{
    // 1. 定义一个窗口类
    static TCHAR szAppName[] = TEXT("GetStarted");
    HWND         hwnd;
    MSG          msg;
    WNDCLASS     wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    // 2. 向系统注册窗口类
    if (!RegisterClass(&wndclass))
    {
        MessageBox(NULL, TEXT("当前程序要求应用于Windows NT!"),
            szAppName, MB_ICONERROR);
        return 0;
    }

    // 3. 创建窗口
    hwnd = CreateWindow(szAppName,                  // window class name
        TEXT("GUI program start!"), // window caption
        WS_OVERLAPPEDWINDOW,        // window style
        0,              // initial x position
        0,              // initial y position
        width,              // initial x size
        height,              // initial y size
        NULL,                       // parent window handle
        NULL,                       // window menu handle
        hInstance,                  // program instance handle
        NULL);                     // creation parameters

    auto textureRender = std::make_shared<D3D11RenderTexture>();
    textureRender->initD3D11(hInstance,hwnd);
 
    // 4. 显示窗口并更新
    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    // 5. 开始进行消息循环， 从消息队列中取出消息进行翻译并分发
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      
        
        textureRender->render();
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC         hdc;
    PAINTSTRUCT ps;
    RECT        rect;

    switch (message)
    {
    case WM_CREATE:

        return 0;

    case WM_PAINT:
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}
