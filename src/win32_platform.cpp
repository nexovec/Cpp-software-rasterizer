#include "win32_platform.h"
#include "common_defines.h"
#include "main.h"
#include <windows.h>
#include <xinput.h>

// FIXME: shame on you
#include <stdio.h>

global BOOL running = true;
global KeyboardState keyboard_state;

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
internal XINPUT_SET_STATE_SIG(XInputSetStateStub)
{
    return 0;
}
global x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

internal void win32InitXInput()
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
internal void pollXInputControllers(unsigned char registered_controllers)
{
    XINPUT_STATE states[4];
    for (int i = 0; i < registered_controllers; i++)
    {
        XINPUT_STATE &pState = states[registered_controllers];
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
}
internal bool win32InitDirectsound()
{
    // ! TODO:
    // LoadLibrary("./dlls/Dsound3d.dll");
    return false;
}
internal void Win32ResizeDibSection(HWND window)
{
    static RECT prev_size;
    RECT window_coords;
    GetWindowRect(window, &window_coords);
    // uint32_t height = prev_size.bottom - prev_size.top;
    uint32_t width = prev_size.right - prev_size.left;
    constexpr double aspect_ratio = 16. / 9.;
    // FIXME: set the client rect to this size instead of the whole window
    SetWindowPos(window, HWND_NOTOPMOST, window_coords.left, window_coords.top, width, (int)((double)width / aspect_ratio), 0);
    prev_size = window_coords;
}
internal void Win32UpdateWindow(HDC device_context, HWND window, BackBuffer back_buffer)
{
    // TODO: benchmark against https://gamedev.net/forums/topic/385918-fast-drawing-to-screen-win32gdi/3552067/
    RECT rect;
    GetClientRect(window, &rect);
    BITMAPINFO bitmap_info = {};
    bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmap_info.bmiHeader.biWidth = default_scene_width;
    bitmap_info.bmiHeader.biHeight = default_scene_height;
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    // TODO: use StretchDIBits and fixed window size
    int res = StretchDIBits(
        device_context,
        0,
        0,
        back_buffer.width,
        back_buffer.height,
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
        break;
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
void dispatchSystemMessages()
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
}
struct file_contents
{
    long size;
    void *data;
    // TODO: manage data lifetime
};
file_contents DEBUGreadWholeFile(char *path)
{
#if defined(DEBUG)
    // TODO: test
    HANDLE file_handle = CreateFile(path, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (file_handle == INVALID_HANDLE_VALUE)
    {
        // TODO: handle can't access file
        OutputDebugStringA("can't access file\n");
        return {};
    }
    file_contents file;
    if (!GetFileSizeEx(file_handle, (PLARGE_INTEGER)&file.size))
    {
        // TODO:  handle can't get file size
        OutputDebugStringA("can't get file size\n");
        return {};
    }
    file.data = VirtualAlloc(0, file.size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!file.data)
    {
        // TODO: handle couldn't allocate file buffer
        OutputDebugStringA("can't allocate buffer for file data\n");
        return {};
    }
    DWORD bytes_read;
    if (!ReadFile(file_handle, file.data, (DWORD)file.size, &bytes_read, 0))
    {
        // TODO: couldn't read file
        OutputDebugStringA("can't read from file\n");
        return {};
    }
    return file;
#else return {};
#endif
}
void DEBUGprintSystemPageSize()
{
#if defined(DEBUG)
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    char print[512];
    sprintf((char *const)&print, "The page size for this system is %u bytes.\n", si.dwPageSize);
    OutputDebugStringA(print);
#endif
}

INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                   _In_ PSTR lpCmdLine, _In_ INT nCmdShow)
{
    DEBUGprintSystemPageSize();
    file_contents test_file_contents = DEBUGreadWholeFile((char *)"test.txt"); // TODO: you need to create test.txt for this to print something
    OutputDebugStringA((char *)test_file_contents.data);
    OutputDebugStringA("^^^ test file was supposed to print here. Did it? ^^^\n");

    // TODO: write to file

    win32InitXInput();
    win32InitDirectsound();
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

    HDC device_context = GetDC(window);
    BackBuffer back_buffer = {};
    back_buffer.width = default_scene_width;
    back_buffer.height = default_scene_height;
    size_t DIB_size = sizeof(uint32_t) * default_scene_width * default_scene_height;
    back_buffer.bits = (uint32_t *)VirtualAlloc(0, DIB_size, MEM_COMMIT, PAGE_READWRITE);
    SetWindowPos(window, HWND_TOP, 300, 180, back_buffer.width, back_buffer.height, 0); // FIXME: weird black stripes

    keyboard_state = {};
    Win32ResizeDibSection(window);
    const double target_fps = 60;
    const double ms_per_tick = 1000.0 / target_fps;
    unsigned long ticks = 0;
    double last_tick = GetTimeMillis();
    unsigned int last_fps = 0;
    double last_fps_log_time = GetTimeMillis();
    while (running)
    {
        double time = GetTimeMillis();
        if (time - last_tick < ms_per_tick)
        {
            time - last_tick > 1 ? Sleep((long)time - last_tick - 1) : Sleep(0);
            // Sleep(0);
            continue;
        }
        else
        {
            dispatchSystemMessages();
            last_tick += ms_per_tick;
            ticks++;
        }
        // OutputDebugStringA("tick!\n");
        // TODO: move this outside of tick to poll more frequently
        // TODO: disable inputs on out of focus
        // TODO: don't poll disconnected controllers
        unsigned char registered_controllers = 1;
        pollXInputControllers(registered_controllers);
        gameUpdateAndRender(back_buffer);
        if (!(GetTimeMillis() - last_tick < ms_per_tick))
        {
// this game update went overbudget
// renders red screen
#ifdef DEBUG
            for (int i = 0; i < back_buffer.width * back_buffer.height; i++)
                back_buffer.bits[i] &= 0xffff0000;
// ? TODO: render "frozen" text in lower left corner instead (disconnect rendering from update??)
// TODO: over a certain treshold, skip frame
// TODO: skip based on average of last n ticks
// TODO: measure skipped frames
#endif
        }
        Win32UpdateWindow(device_context, window, back_buffer);

        last_fps++;
        if (GetTimeMillis() - last_fps_log_time > 1000)
        {
            char title[128];
            sprintf((char *const)&title, "handmade_test | fps: %u", last_fps);
            SetWindowText(window, title);
            last_fps = 0;
            last_fps_log_time = GetTimeMillis();
        }
    }
    return 0;
}
