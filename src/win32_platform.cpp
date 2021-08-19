#include "win32_platform.h"
#include "common_defines.h"
#include "main.h"
#include <windows.h>
#include <stdio.h> // FIXME: no stl, you lazy goddarn pig

global BOOL running = true;
HDC device_context;


internal void win32_resize_dib_section(HWND window)
{
    // TODO: resize window, but retain aspect ratio
    RECT window_coords;
    GetWindowRect(window, &window_coords);
    SetWindowPos(window, HWND_NOTOPMOST, window_coords.left, window_coords.top, 800, 600, 0);
}
internal void win32_update_window(HDC device_context, HWND window, BACK_BUFFER back_buffer, BITMAPINFO bitmap_info)
{
    // TODO: benchmark against https://gamedev.net/forums/topic/385918-fast-drawing-to-screen-win32gdi/3552067/
    RECT rect;
    GetClientRect(window, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    int res = StretchDIBits(
        device_context,
        0,
        0,
        width,
        height,
        0,
        0,
        back_buffer.width,
        back_buffer.height,
        back_buffer.bits,
        &bitmap_info,
        DIB_RGB_COLORS,
        SRCCOPY);
}

LRESULT CALLBACK window_proc(
    HWND window,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    LRESULT result = 0;
    // FIXME: make device context global
    switch (uMsg)
    {
    case WM_SIZE:
    {
        win32_resize_dib_section(window);
        OutputDebugStringA("WM_SIZE\n");
    }
    break;
    case WM_CLOSE:
    {
        OutputDebugStringA("WM_CLOSE\n");
        exit(0);
    }
    break;
    case WM_DESTROY:
    {
        OutputDebugStringA("WM_DESTROY\n");
    }
    break;
    case WM_ACTIVATEAPP:
    {
        OutputDebugStringA("WM_ACTIVATEAPP\n");
    }
    break;
    case WM_PAINT:
    {
        OutputDebugStringA("WM_PAINT\n");
        win32_update_window(device_context, window_rect);
    }
    break;
    default:
    {
        result = DefWindowProc(window, uMsg, wParam, lParam);
    }
    break;
    }
    return result;
}
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR lpCmdLine, INT nCmdShow)
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    char print[512];
    sprintf((char *const)&print, "The page size for this system is %u bytes.\n", si.dwPageSize);
    OutputDebugStringA(print);

    WNDCLASSEXA window_class_ex = {};
    window_class_ex.cbSize = sizeof(WNDCLASSEX);
    window_class_ex.style = CS_OWNDC;
    window_class_ex.lpfnWndProc = &window_proc;
    window_class_ex.hInstance = hInstance;
    window_class_ex.hbrBackground = 0;
    window_class_ex.lpszClassName = "SuperWindowClass";
    ATOM window_class_atom = RegisterClassEx(&window_class_ex);
    if (!window_class_atom)
        return -1;
    HWND window = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        window_class_ex.lpszClassName,
        "This is my window!",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL);
    if (!window)
    {
        return -1;
        // TODO: logging
    }
    SetWindowPos(window, HWND_TOP, 400, 280, 800, 600, 0);
    device_context = GetDC(window);
    BACK_BUFFER back_buffer = {};
    back_buffer.width = scene_width;
    back_buffer.height = scene_height;
    size_t DIB_size = sizeof(uint32_t) * scene_width * scene_height;
    back_buffer.bits = (uint32_t *)VirtualAlloc(0, DIB_size, MEM_COMMIT, PAGE_READWRITE);
    BITMAPINFO bitmap_info = {};
    bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmap_info.bmiHeader.biWidth = scene_width;
    bitmap_info.bmiHeader.biHeight = scene_height;
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;

    win32_resize_dib_section(window);
    while (running)
    {
        MSG message;
        BOOL bRet = GetMessage(
            &message,
            0,
            0,
            0);
        if (bRet == -1)
        {
            return -1;
        }
        TranslateMessage(&message);
        DispatchMessage(&message);
        game_update_and_render(back_buffer);
        win32_update_window(device_context, window, back_buffer, bitmap_info);
    }
    return 0;
}