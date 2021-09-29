// FIXME: get rid of big imports
#include "common_defines.hpp"
#include "platform_layer.hpp"
#include "data_parsing.hpp"
#include "main.hpp"
#include <windows.h>
#include <xinput.h>
#include <stdio.h>

global BOOL running = true;
global KeyboardState keyboard_state;

#define XINPUT_GET_STATE_SIG(name) uint64 name(uint64, XINPUT_STATE *)
typedef XINPUT_GET_STATE_SIG(x_input_get_state);
XINPUT_GET_STATE_SIG(XInputGetStateStub)
{
    return 0;
}
global x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

#define XINPUT_SET_STATE_SIG(name) uint64 name(uint64, XINPUT_VIBRATION *)
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
    for (int32 i = 0; i < registered_controllers; i++)
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
internal int32 Win32UpdateWindow(HDC device_context, HWND window, BackBuffer back_buffer)
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
    // int32 width = rect.right - rect.left;
    // int32 height = rect.bottom - rect.top;
    // TODO: use StretchDIBits and fixed window size
    int32 res = StretchDIBits(
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
internal int32 HandleKeypress(WPARAM wParam, LPARAM lParam, bool is_down)
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
internal real64 GetTimeMillis()
{
    LARGE_INTEGER lpPerformanceCount;
    if (!QueryPerformanceCounter(&lpPerformanceCount))
    {
        // TODO: error handle
        ExitProcess(1);
    }
    persistent LARGE_INTEGER lpFrequency;
    if (!QueryPerformanceFrequency(&lpFrequency))
    {
        // TODO: error handle
        ExitProcess(1);
    }
    real64 time_in_seconds = (real64)lpPerformanceCount.QuadPart / (real64)lpFrequency.QuadPart;
    return time_in_seconds * 1000;
}

internal LRESULT CALLBACK WindowProc(
    HWND window,
    uint32 uMsg,
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
        ExitProcess(1);
    }
    TranslateMessage(&message);
    DispatchMessage(&message);
}
void file_contents::free()
{
    VirtualFree(this->data, 0, MEM_RELEASE);
}
file_contents file_contents::readWholeFile(char *path, uint64 min_allocd_size)
{
#ifdef DEBUG
    OutputDebugStringA(path);
    OutputDebugStringA("\n");
#endif
    HANDLE file_handle = CreateFile(path, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    file_contents file = {};
    if (file_handle == INVALID_HANDLE_VALUE)
    {
        OutputDebugStringA("can't access file\n");
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

    uint64 bytes_read;
    if (!file.data)
    {
        OutputDebugStringA("can't allocate buffer for file data\n");
    }
    else if (!(file.size <= 0xffffffff))
    {
        OutputDebugStringA("file is too big");
    }
    else if (!ReadFile(file_handle, file.data, (uint32)file.size, (LPDWORD)&bytes_read, 0))
    {
        OutputDebugStringA("can't read from file\n");
    }
    else if ((bytes_read != file.size))
    {
        OutputDebugStringA("wrong size read from file");
    }
    else
    {
        CloseHandle(file_handle);
        return file;
    }
    OutputDebugStringA("Something went wrong opening a file");
    CloseHandle(file_handle);
    VirtualFree(file.data, 0, MEM_RELEASE);
    return {};
}
void DEBUGprintSystemPageSize()
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
struct Assets
{
    Assets();
    BitmapImage font_image;
    // ? FIXME: optional memory free
};
struct TileMap
{
    // FIXME: have generic image class
    BitmapImage imageData;
    uint32 tile_width;
    uint32 tile_height;
    uint32 tiles_per_width;
    uint32 tiles_per_height;
    TileMap(BitmapImage imageData, uint32 tile_width, uint32 tile_height);
    void DEBUGdraw(BackBuffer *back_buffer, int32 x, int32 y, int32 x_offset, int32 y_offset);
    void DEBUGrenderBitmapText(BackBuffer *back_buffer, char *text, int32 x_offset, int32 y_offset);
};
TileMap::TileMap(BitmapImage imageData, uint32 tile_width, uint32 tile_height)
{
    this->imageData = imageData;
    this->tile_width = tile_width;
    this->tile_height = tile_height;
    this->tiles_per_height = imageData.bh->bmp_info_header.Height / this->tile_height;
    this->tiles_per_width = imageData.bh->bmp_info_header.Width / this->tile_width;
}
uint32 alphaBlendColors(uint32 colora, uint32 colorb)
{
    // RESEARCH: bit shift has cycling?? what happens to the blue channel?
    uint32 alpha = colora >> 24;
    // uint32 alpha = 10;
    uint32 rb1 = ((0x100 - alpha) * (colora & 0xFF00FF)) >> 8;
    uint32 rb2 = (alpha * (colorb & 0xFF00FF)) >> 8;
    uint32 g1 = ((0x100 - alpha) * (colora & 0x00FF00)) >> 8;
    uint32 g2 = (alpha * (colorb & 0x00FF00)) >> 8;
    return ((rb1 | rb2) & 0xFF00FF) + ((g1 | g2) & 0x00FF00);
}
void DEBUGBltBmp(BackBuffer *back_buffer, BitmapImage bmp, int32 x_offset, int32 y_offset)
{
    for (int32 x = 0; x < bmp.bh->bmp_info_header.Width; x++)
    {
        for (int32 y = 0; y < bmp.bh->bmp_info_header.Height; y++)
        {
            back_buffer->bits[back_buffer->width * (y + y_offset) + x + x_offset] = alphaBlendColors(bmp.pixels[y * bmp.bh->bmp_info_header.Width + x], back_buffer->bits[back_buffer->width * (y + y_offset) + x + x_offset]);
        }
    }
}
void TileMap::DEBUGdraw(BackBuffer *back_buffer, int32 x, int32 y, int32 x_offset, int32 y_offset)
{
    // FIXME: access violation on negative offsets
    // FIXME: This is copying bmp headers
    BitmapImage bmp = this->imageData;
    for (int32 xx = 0; xx < this->tile_width; xx++)
    {
        for (int32 yy = 0; yy < this->tile_height; yy++)
        {
            // HACK: the indexing, LUL.
            uint32 new_color = bmp.pixels[((this->tiles_per_height - 1 - y) * this->tile_height + yy) * bmp.bh->bmp_info_header.Width + x * this->tile_width + xx];
            back_buffer->bits[back_buffer->width * (yy + y_offset) + xx + x_offset] = alphaBlendColors(new_color, back_buffer->bits[back_buffer->width * (yy + y_offset) + xx + x_offset]);
        }
    }
}
void TileMap::DEBUGrenderBitmapText(BackBuffer *back_buffer, char *text, int32 x_offset, int32 y_offset)
{
    // TODO: test with 0 length text
    // TODO: support multi-line text
    // TODO: auto-allign
    // NOTE: this works with ASCII only 32x4 tilemapped bitmap fonts. Their resolution is supplied on init.
    // FIXME: can try to access unsupported characters
    int32 index = 0;
    int32 character = 'a';
    do
    {
        character = (int32)text[index];
        // this->DEBUGdraw(back_buffer,character%this->tiles_per_width,character/this->tiles_per_width,x_offset+index*this->tile_width,y_offset);
        // FIXME: indexed bottom
        this->DEBUGdraw(back_buffer, character % 32, character / 32, x_offset + index * this->tile_width, y_offset);
        index++;
    } while (text[index] != '\0');
}
Assets::Assets()
{
    int8 *path = (int8 *)"font.bmp";
    // this->font_image = BitmapImage::loadBmpFromFile(path);
    BitmapImage::loadBmpFromFile(&this->font_image, path);
    // FIXME: this needs to get called, otherwise it throws?!
    this->font_image.setOpaquenessTo(0x22000000);
    // FIXME: no safeguard against read errors
    return;
}
// void DEBUGBltBmp(BackBuffer &back_buffer, BitmapImage &bmp, int32 x_offset = 0, int32 y_offset = 0);
int32 WINAPI WinMain(HINSTANCE hInstance, HINSTANCE,
                     PSTR, int32)
{
    DEBUGprintSystemPageSize();
    {
#ifdef DEBUG
        file_contents test_file_contents = file_contents::readWholeFile((char *)"test.txt"); // TODO: you need to create test.txt for this to print something
        OutputDebugStringA((char *)test_file_contents.data);
        OutputDebugStringA("\n^^^ test file was supposed to print here. Did it? ^^^\n");
#endif
    }

    Assets assets = Assets();
    TileMap font_tile_map = TileMap(assets.font_image, 512 / 32, 96 / 4);

    {
#ifdef DEBUG
        int32 h = (int32)assets.font_image.bh->bmp_info_header.Height;
        char buf[128];
        sprintf_s(buf, 128, "%ld\n", h);
        OutputDebugStringA(buf);
#endif
    }

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
    SetFocus(window);
    // FIXME: this gets multiplied by windows global scale multiplier for some reason:
    // TODO: center window on screen

    HDC device_context = GetDC(window);
    BackBuffer back_buffer = {};
    back_buffer.width = default_scene_width;
    back_buffer.height = default_scene_height;
    memory_index DIB_size = sizeof(uint32) * default_scene_width * default_scene_height;
    back_buffer.bits = (uint32 *)VirtualAlloc(0, DIB_size, MEM_COMMIT, PAGE_READWRITE);
    // FIXME: weird window style
    SetWindowPos(window, HWND_TOP, 300, 180, back_buffer.width + 20, back_buffer.height + 40, 0);
    keyboard_state = {};
    const real64 target_fps = 60;
    const real64 ms_per_tick = 1000.0 / target_fps;
    uint64 ticks = 0;
    real64 last_tick = GetTimeMillis();
    uint32 last_fps = 0;
    real64 last_fps_log_time = GetTimeMillis();
    while (running)
    {
        real64 time = GetTimeMillis();
        if (time - last_tick < ms_per_tick)
        {
            time - last_tick > 1 ? Sleep((int32)(time - last_tick - 1)) : Sleep(0);
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
        // PERFORMANCE: I suspect performance problems when polling XInput controllers(measure, fix, add other controller API?)
        // TODO: move this outside of tick to poll more frequently (test if it helps)
        // TODO: disable inputs on out of focus
        // TODO: don't poll disconnected controllers
        unsigned char registered_controllers = 1;
        pollXInputControllers(registered_controllers);
        gameUpdateAndRender(back_buffer);
#ifndef DEBUG
        static_assert(false);
#endif
        DEBUGBltBmp(&back_buffer, assets.font_image, 100, 200);
        // font_tile_map.DEBUGdraw(&back_buffer, 6, 1, 200, 400);
        font_tile_map.DEBUGrenderBitmapText(&back_buffer, (char *)"Fabulous!", 200, 400);
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
