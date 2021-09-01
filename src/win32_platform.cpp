#include "win32_platform.h"
#include "common_defines.h"
#include "main.h"
#include <windows.h>
#include <xinput.h>
#include <stdio.h> // FIXME: no stl, you lazy goddarn pig

global BOOL running = true;
global Keyboard_state keyboard_state;
global double aspect_ratio = 4. / 3.;
global RECT prev_size;

#define XINPUT_GET_STATE_SIG(name) DWORD name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef XINPUT_GET_STATE_SIG(x_input_get_state);
XINPUT_GET_STATE_SIG(XInputGetStateStub)
{
    return 0;
}
global x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

#define XINPUT_SET_STATE_SIG(name) DWORD name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef XINPUT_SET_STATE_SIG(x_input_set_state);
XINPUT_SET_STATE_SIG(XInputSetStateStub)
{
    return 0;
}
global x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

internal void Win32InitXInput()
{
    HMODULE lib = LoadLibrary("xinput1_4.dll");
    if (!lib)
    {
        lib = LoadLibrary("xinput1_3.dll");
    }
    if (!lib)
    {
        ExitProcess(-1);
    }
    XInputSetState_ = (x_input_set_state *)GetProcAddress(lib, "XInputSetState");
    XInputGetState_ = (x_input_get_state *)GetProcAddress(lib, "XInputGetState");
}
internal bool Win32InitDirectsound()
{
    // ! TODO:
    // LoadLibrary("./dlls/Dsound3d.dll");
    return false;
}
internal void Win32ResizeDibSection(HWND window)
{
    RECT window_coords;
    GetWindowRect(window, &window_coords);
    // uint32_t height = prev_size.bottom - prev_size.top;
    uint32_t width = prev_size.right - prev_size.left;
    SetWindowPos(window, HWND_NOTOPMOST, window_coords.left, window_coords.top, width, (int)((double)width / aspect_ratio), 0);
    prev_size = window_coords;
}
internal void Win32UpdateWindow(HDC device_context, HWND window, Back_buffer back_buffer)
{
    // TODO: benchmark against https://gamedev.net/forums/topic/385918-fast-drawing-to-screen-win32gdi/3552067/
    RECT rect;
    GetClientRect(window, &rect);
    BITMAPINFO bitmap_info = {};
    bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmap_info.bmiHeader.biWidth = scene_width;
    bitmap_info.bmiHeader.biHeight = scene_height;
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;
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
internal int HandleKeypress(WPARAM wParam, LPARAM lParam, bool is_down)
{
    if (is_down && lParam & (1 << 30))
        return 0;
    if (is_down)
    {
        OutputDebugStringA("Key has been pressed\n");
    }
    else
    {
        OutputDebugStringA("Key has been released\n");
    }
    switch (wParam)
    {
    case VK_SPACE:
    {
        keyboard_state.KEY_SPACE = is_down;
    }
    break;
    case 'W':
    {
        keyboard_state.KEY_W = is_down;
    }
    break;
    case 'A':
    {
        keyboard_state.KEY_A = is_down;
    }
    break;
    case 'S':
    {
        keyboard_state.KEY_S = is_down;
    }
    break;
    case 'D':
    {
        keyboard_state.KEY_D = is_down;
    }
    break;
    default:
        break;
    }
    if (lParam & (1 << 29) && wParam == VK_F4)
    {
        running = false;
    }
    return 0;
}
internal double GetTimeMillis()
{
    LARGE_INTEGER lpPerformanceCount;
    if (!QueryPerformanceCounter(&lpPerformanceCount))
    {
        // TODO: error handle
        ExitProcess(-1);
    }
    persistent LARGE_INTEGER lpFrequency;
    if (!QueryPerformanceFrequency(&lpFrequency))
    {
        // TODO: error handle
        ExitProcess(-1);
    }
    double time_in_seconds = (double)lpPerformanceCount.QuadPart / (double)lpFrequency.QuadPart;
    return time_in_seconds * 1000;
}

internal LRESULT CALLBACK WindowProc(
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
        Win32ResizeDibSection(window);
        RedrawWindow(window, NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);
        OutputDebugStringA("WM_SIZE\n");
    }
    break;
    case WM_PAINT:
    {
        // OutputDebugStringA("WM_PAINT\n");
        // TODO: remove, make window non-resizable
        // PAINTSTRUCT ps;
        // HDC hdc = BeginPaint(window, &ps);
        // // FIXME: this is slow
        // persistent HBITMAP bmp = CreateBitmap(scene_width, scene_height, 1, 32, back_buffer.bits);
        // persistent HBRUSH back_buffer_brush = CreatePatternBrush(bmp);
        // FillRect(hdc, &ps.rcPaint, back_buffer_brush);
        // DeleteObject(bmp);
        // EndPaint(window, &ps);
    }
    break;
    case WM_CLOSE:
    {
        OutputDebugStringA("WM_CLOSE\n");
        running = false;
    }
    break;
    case WM_DESTROY:
    {
        OutputDebugStringA("WM_DESTROY\n");
        running = false;
    }
    break;
    case WM_ACTIVATEAPP:
    {
        OutputDebugStringA("WM_ACTIVATEAPP\n");
    }
    break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    {
        return HandleKeypress(wParam, lParam, 1);
    }
    break;
    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        return HandleKeypress(wParam, lParam, 0);
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
INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                   _In_ PSTR lpCmdLine, _In_ INT nCmdShow)
{
    // ! DEBUG:
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    char print[512];
    sprintf((char *const)&print, "The page size for this system is %u bytes.\n", si.dwPageSize);
    OutputDebugStringA(print);
    // !

    Win32InitXInput();
    Win32InitDirectsound();
    WNDCLASSEXA window_class_ex = {};
    window_class_ex.cbSize = sizeof(WNDCLASSEX);
    window_class_ex.style = CS_OWNDC;
    window_class_ex.lpfnWndProc = &WindowProc;
    window_class_ex.hInstance = hInstance;
    window_class_ex.hbrBackground = 0;
    window_class_ex.lpszClassName = "SuperWindowClass";
    ATOM window_class_atom = RegisterClassEx(&window_class_ex);
    if (!window_class_atom)
        ExitProcess(-1);
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
        // TODO: error handle
        ExitProcess(-1);
    }
    SetFocus(window);
    // FIXME: this gets multiplied by windows global scale multiplier for some reason:
    // TODO: center window on screen
    SetWindowPos(window, HWND_TOP, 300, 180, 800, 600, 0);
    HDC device_context = GetDC(window);
    Back_buffer back_buffer = {};
    back_buffer.width = scene_width;
    back_buffer.height = scene_height;
    size_t DIB_size = sizeof(uint32_t) * scene_width * scene_height;
    back_buffer.bits = (uint32_t *)VirtualAlloc(0, DIB_size, MEM_COMMIT, PAGE_READWRITE);

    keyboard_state = {};
    Win32ResizeDibSection(window);
    double target_fps = 60;
    double ms_per_tick = 1000.0 / target_fps;
    double last_tick = 0;
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
            ExitProcess(-1);
        }
        TranslateMessage(&message);
        DispatchMessage(&message);
        double time = GetTimeMillis();
        if (time - last_tick < ms_per_tick)
        {
            time - last_tick > 1 ? Sleep((long)((time - last_tick) / 1) - 1) : Sleep(0);
            // Sleep(1);
            continue;
        }
        // OutputDebugStringA("tick!\n");
        last_tick = time;
        unsigned char registered_controllers = 1;
        // TODO: move this outside of tick to poll more frequently
        for (int i = 0; i < registered_controllers; i++)
        {
            XINPUT_STATE pState;
            XINPUT_GAMEPAD *controller_state = &pState.Gamepad;
            XInputGetState(i, &pState);
            // ! DEBUG:
            XINPUT_VIBRATION controller_vibration = {GAMEPAD_RUMBLE_LEVEL::OFF, GAMEPAD_RUMBLE_LEVEL::OFF};
            if (controller_state->bRightTrigger)
            {
                controller_vibration.wLeftMotorSpeed = GAMEPAD_RUMBLE_LEVEL::LEVEL_2;
                controller_vibration.wLeftMotorSpeed = GAMEPAD_RUMBLE_LEVEL::LEVEL_2;
            }
            XInputSetState(i, &controller_vibration);
            // !
        }
        // TODO: disable inputs on out of focus
        // TODO: don't poll disconnected controllers
        GameUpdateAndRender(back_buffer);
        Win32UpdateWindow(device_context, window, back_buffer);
    }
    return 0;
}
