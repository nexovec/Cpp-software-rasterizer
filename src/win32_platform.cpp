#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#define global static
#define internal static
#define persistent static

global BOOL running = true;
global BITMAPINFO bitmap_info;
global uint32_t *bitmap_memory = NULL;
global RECT window_rect;
global LONG scene_width = 800;
global LONG scene_height = 600;
global LONG last_width = 800;
global LONG last_height = 800;

int sprintf(
    char *buffer,
    const char *format, ...);

internal void temp_redraw_buffer()
{
    for (int i = 0; i < scene_height; i++)
    {
        for (int ii = 0; ii < scene_width; ii++)
        {
            bitmap_memory[ii + i * scene_width] = 0xffaaff;
        }
    }
}
internal void win32_resize_dib_section(HDC dc, HWND window, RECT rect)
{
    LONG width = rect.right - rect.left;
    LONG height = rect.bottom - rect.top;
    bitmap_info = {};
    BITMAPINFOHEADER *h = &bitmap_info.bmiHeader;
    h->biSize = sizeof(BITMAPINFOHEADER);
    h->biWidth = scene_width;
    h->biHeight = scene_height;
    h->biPlanes = 1;
    h->biBitCount = 32;
    h->biCompression = BI_RGB;

    if (!bitmap_memory)
    {
        // VirtualFree(bitmap_memory, 0, MEM_RELEASE);
        size_t DIB_size = sizeof(uint32_t) * scene_width * scene_height;
        bitmap_memory = (uint32_t *)VirtualAlloc(0, DIB_size, MEM_COMMIT, PAGE_READWRITE);
    }
    RECT window_coords;
    GetWindowRect(window, &window_coords);
    SetWindowPos(window, HWND_TOPMOST, window_coords.left, window_coords.top, 800, 600, 0);
}
internal void win32_update_window(HDC device_context, RECT rect)
{
    int width = rect.right - rect.left;
    assert(width >= 0);
    int height = rect.bottom - rect.top;
    assert(height >= 0);

    int res = StretchDIBits(
        device_context,
        0,
        0,
        width,
        height,
        0,
        0,
        scene_width,
        scene_height,
        bitmap_memory,
        &bitmap_info,
        DIB_RGB_COLORS,
        SRCCOPY);
    // ReleaseDC(window, device_context);
}

LRESULT CALLBACK window_proc(
    HWND window,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    LRESULT result = 0;
    // FIXME: make device context global
    HDC device_context = GetDC(window);
    switch (uMsg)
    {
    case WM_SIZE:
    {
        GetClientRect(window, &window_rect);
        // TODO: resize window, but retain aspect ratio
        // TODO: resize window, but retain middle of the window in the same position
        // TODO: style
        // FIXME: update is called twice
        win32_resize_dib_section(device_context, window, window_rect);
        win32_update_window(device_context, window_rect);
        // OutputDebugStringA("WM_SIZE\n");
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
        win32_update_window(device_context, window_rect);
    }
    break;
    default:
    {
        result = DefWindowProc(window, uMsg, wParam, lParam);
    }
    break;
    }
    ReleaseDC(window, device_context);
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
    window_class_ex.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
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
    HDC device_context = GetDC(window);
    GetClientRect(window, &window_rect);

    win32_resize_dib_section(device_context, window, window_rect);
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
        temp_redraw_buffer();
        win32_update_window(device_context, window_rect);
    }
    return 0;
}