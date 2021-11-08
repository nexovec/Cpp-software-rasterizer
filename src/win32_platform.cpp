// FIXME: get rid of big imports
#include "common_defines.hpp"
#include "platform_layer.hpp"
#include "data_parsing.hpp"
#include "main.hpp"
#undef UNICODE
#include <windows.h>
#include <xinput.h>
#include <stdio.h>
#include <stdio.h>
global BOOL running = true;
global KeyboardState keyboard_state;

#define XINPUT_GET_STATE_SIG(name) uint_64 name(uint_64, XINPUT_STATE *)
typedef XINPUT_GET_STATE_SIG(x_input_get_state);
XINPUT_GET_STATE_SIG(XInputGetStateStub)
{
    return 0;
}
global x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

#define XINPUT_SET_STATE_SIG(name) uint_64 name(uint_64, XINPUT_VIBRATION *)
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
        ExitProcess(1);
    }
    XInputSetState_ = (x_input_set_state *)GetProcAddress(lib, "XInputSetState");
    XInputGetState_ = (x_input_get_state *)GetProcAddress(lib, "XInputGetState");
}
internal void pollXInputControllers(unsigned char registered_controllers)
{
    XINPUT_STATE states[4];
    for (int_32 i = 0; i < registered_controllers; i++)
    {
        XINPUT_STATE &pState = states[registered_controllers];
        XINPUT_GAMEPAD *controller_state = &pState.Gamepad;
        XInputGetState(i, &pState);
        // DEBUG:
        XINPUT_VIBRATION controller_vibration = {GAMEPAD_RUMBLE_LEVEL::OFF, GAMEPAD_RUMBLE_LEVEL::OFF};
        if (controller_state->bRightTrigger)
        {
            controller_vibration.wLeftMotorSpeed = GAMEPAD_RUMBLE_LEVEL::LEVEL_2;
            controller_vibration.wLeftMotorSpeed = GAMEPAD_RUMBLE_LEVEL::LEVEL_2;
        }
        XInputSetState(i, &controller_vibration);
    }
}
internal bool win32InitDirectsound()
{
    // TODO: sound
    // LoadLibrary("./dlls/Dsound3d.dll");
    return false;
}
internal int_32 Win32UpdateWindow(HDC device_context, HWND window, argb_texture back_buffer)
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
    // int_32 width = rect.right - rect.left;
    // int_32 height = rect.bottom - rect.top;
    // TODO: use StretchDIBits and fixed window size
    int_32 res = StretchDIBits(
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
    return res;
}
internal int_32 HandleKeypress(WPARAM wParam, LPARAM lParam, bool is_down)
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
LARGE_INTEGER lpFrequency;
internal real_64 GetTimeMillis()
{
    LARGE_INTEGER lpPerformanceCount;
    if (!QueryPerformanceCounter(&lpPerformanceCount))
    {
        // TODO: error handle
        ExitProcess(1);
    }
    if (!QueryPerformanceFrequency(&lpFrequency))
    {
        // TODO: error handle
        ExitProcess(1);
    }
    real_64 time_in_seconds = (real_64)lpPerformanceCount.QuadPart / (real_64)lpFrequency.QuadPart;
    return time_in_seconds * 1000;
}

internal LRESULT CALLBACK WindowProc(
    HWND window,
    uint_32 uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    LRESULT result = 0;
    // FIXME: make device context global
    switch (uMsg)
    {
    case WM_SIZE:
    {
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
internal void dispatchSystemMessages()
{
    MSG message;
    BOOL bRet = GetMessage(
        &message,
        0,
        0,
        0);
    if (bRet == -1)
    {
        ExitProcess(1);
    }
    TranslateMessage(&message);
    DispatchMessage(&message);
}
void file_contents::free()
{
    VirtualFree(this->data, 0, MEM_RELEASE);
}
file_contents file_contents::readWholeFile(char *path, uint_64 min_allocd_size)
{
#ifdef DEBUG
    OutputDebugStringA(path);
    OutputDebugStringA("\n");
#endif
    HANDLE file_handle = CreateFile(path, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    file_contents file = {};
    if (file_handle == INVALID_HANDLE_VALUE)
    {
        OutputDebugStringA("Can't access file\n");
        CloseHandle(file_handle);
        return {};
    }
    if (!GetFileSizeEx(file_handle, (PLARGE_INTEGER)&file.size))
    {
        OutputDebugStringA("can't get file size\n");
        CloseHandle(file_handle);
        return {};
    }
    file.size = max(file.size, min_allocd_size);
    file.data = VirtualAlloc(0, file.size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    uint_32 bytes_read;
    if (!file.data)
    {
        OutputDebugStringA("Can't allocate buffer for file data\n");
    }
    else if (!(file.size <= 0xffffffff))
    {
        OutputDebugStringA("File is too big\n");
    }
    else if (!ReadFile(file_handle, file.data, (uint_32)file.size, (LPDWORD)&bytes_read, 0))
    {
        OutputDebugStringA("Can't read from file\n");
    }
    else if ((bytes_read != file.size))
    {
        char *log_message[512];
        sprintf_s((char *)log_message, 512, "Wrong size read from file (%llu expected, got %llu)\n", file.size, (uint_64)bytes_read);
        OutputDebugStringA((LPCSTR)log_message);
    }
    else
    {
        CloseHandle(file_handle);
        return file;
    }
    OutputDebugStringA("^^^ Something went wrong opening a file\n");
    CloseHandle(file_handle);
    VirtualFree(file.data, 0, MEM_RELEASE);
    return {};
}
internal void DEBUGprintSystemPageSize()
{
#if defined(DEBUG)
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    char print[512];
    sprintf_s((char *)&print, 512, "The page size for this system is %u bytes.\n", si.dwPageSize);
    OutputDebugStringA(print);
#endif
}
void TerminateProcess(int ret_code)
{
    __debugbreak();
    ExitProcess(ret_code);
}

// TODO: write to file

struct RuntimeThreadParams
{
    HWND window;
    argb_texture back_buffer;
    HDC device_context;
};
internal DWORD WINAPI runtimeThreadProc(LPVOID lpParam)
{
    RuntimeThreadParams params = *(RuntimeThreadParams *)lpParam;
    HWND window = params.window;
    argb_texture back_buffer = params.back_buffer;
    HDC device_context = params.device_context;
    constexpr real_64 target_fps = 60.0;
    constexpr real_64 ms_per_tick = 1000.0 / target_fps;
    uint_64 ticks = 0;
    real_64 last_tick = GetTimeMillis();
    uint_32 last_fps = 0;
    real_64 last_fps_log_time = GetTimeMillis();
    while (running)
    {
        real_64 time = GetTimeMillis();
        if (time - last_tick < ms_per_tick)
        {
            // time - last_tick > 1 ? Sleep((int_32)(time - last_tick - 1)) : Sleep(0);
            Sleep(0);
            continue;
        }
        last_tick += ms_per_tick;
        ticks++;
        // OutputDebugStringA("tick!\n");
        // PERFORMANCE: I suspect performance problems when polling XInput controllers(measure, fix, add other controller API?)
        // TODO: disable inputs on out of focus
        // TODO: don't poll disconnected controllers
        unsigned char registered_controllers = 1;
        pollXInputControllers(registered_controllers);
        gameUpdateAndRender(back_buffer);
        Win32UpdateWindow(device_context, window, back_buffer);

        last_fps++;
        if (GetTimeMillis() - last_fps_log_time > 1000)
        {
            char title[128];
            sprintf_s((char *const)&title, 128, "handmade_test | fps: %u", last_fps);
            SetWindowText(window, title);
            last_fps = 0;
            last_fps_log_time = GetTimeMillis();
        }
    }
    return 0;
}

int_32 WINAPI WinMain(HINSTANCE hInstance, HINSTANCE,
                      PSTR, int_32)
{
#ifdef DEBUG
    DEBUGprintSystemPageSize();
    {
        // TODO: search for assets folder
        file_contents test_file_contents = file_contents::readWholeFile((char *)"assets/test.txt");
        OutputDebugStringA((char *)test_file_contents.data);
        OutputDebugStringA("\n^^^ test file was supposed to print here. Did it? ^^^\n");
    }
#endif

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
        ExitProcess(1);
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
        ExitProcess(1);
    }
    // FIXME: this gets multiplied by windows global scale multiplier for some reason:
    // FIXME: client area isn't scaled properly with respect to the window size, investigate
    // TODO: center window on screen
    // TODO: make window unresizable
    SetFocus(window);
    SetWindowPos(window, HWND_TOP, 300, 180, default_scene_width + 20, default_scene_height + 40, 0);

    HDC device_context = GetDC(window);
    argb_texture back_buffer = {};
    back_buffer.width = default_scene_width;
    back_buffer.height = default_scene_height;
    memory_index DIB_size = sizeof(uint_32) * default_scene_width * default_scene_height;
    back_buffer.bits = (uint_32 *)VirtualAlloc(0, DIB_size, MEM_COMMIT, PAGE_READWRITE);

    // start runtime thread
    // NOTE: this is done in order to prevent window from freezing when being moved
    DWORD thread_id;
    RuntimeThreadParams params = RuntimeThreadParams{window, back_buffer, device_context};
    HANDLE thread_handle = CreateThread(NULL, 0, runtimeThreadProc, (LPVOID)&params, 0, &thread_id);

    // do window management
    constexpr real_64 target_fps = 20;
    constexpr real_64 ms_per_tick = 1000.0 / target_fps;
    uint_64 ticks = 0;
    real_64 last_tick = GetTimeMillis();
    while (running)
    {
        real_64 time = GetTimeMillis();
        if (time - last_tick < ms_per_tick)
        {
            time - last_tick > 1 ? Sleep((int_32)(time - last_tick - 1)) : Sleep(0);
            continue;
        }
        else
        {
            dispatchSystemMessages();
            last_tick += ms_per_tick;
            ticks++;
        }
    }
    return 0;
}
