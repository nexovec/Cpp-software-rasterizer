#include "./../src/common_defines.hpp"
#include "./../src/platform_layer.hpp"
#include <windows.h>
// TODO: slim down

bool running = true;
global KeyboardState keyboard_state;

struct Vec2f
{
    real32 x;
    real32 y;
};
struct Triangle2D
{
    Vec2f v1;
    Vec2f v2;
    Vec2f v3;
};
template <typename T>
T sgn(T, T = 0);
template <typename T>
inline T sgn(T n, T zero)
{
    return (n > 0) - (n < 0);
}
inline real32 lerp(real32 a, real32 b, real32 ratio)
{
    return a + ratio * (b - a);
}
inline real32 invLerp(real32 a, real32 b, real32 val)
{
    return (val - a) / (b - a);
}
#define back_buffer(x, y) back_buffer.bits[back_buffer.width * y + x]
void clearScreen(BackBuffer back_buffer)
{
    for (uint32 i = 0; i < back_buffer.height; i++)
    {
        for (uint32 ii = 0; ii < back_buffer.width; ii++)
        {
            back_buffer(ii, i) = 0xffaaff;
        }
    }
}
uint32 interpolatedColor(Triangle2D triangle, real32 x, real32 y, uint32 color1, uint32 color2, uint32 color3)
{
    // use solid color
    // return 0xff00ff00;
    Vec2f v1 = triangle.v1;
    Vec2f v2 = triangle.v2;
    Vec2f v3 = triangle.v3;
    // get barycentric coordinates
    real32 det_t = (v2.y - v3.y) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.y - v3.y);
    real32 lam_1 = ((v2.y - v3.y) * (x - v3.x) + (v3.x - v2.x) * (y - v3.y)) / det_t;
    real32 lam_2 = ((v3.y - v1.y) * (x - v3.x) + (v1.x - v3.x) * (y - v3.y)) / det_t;
    real32 lam_3 = 1 - lam_2 - lam_1;

    uint8 a1 = (uint8)(color1 >> 24);
    uint8 r1 = (uint8)(color1 >> 16);
    uint8 g1 = (uint8)(color1 >> 8);
    uint8 b1 = (uint8)(color1 >> 0);

    uint8 a2 = (uint8)(color2 >> 24);
    uint8 r2 = (uint8)(color2 >> 16);
    uint8 g2 = (uint8)(color2 >> 8);
    uint8 b2 = (uint8)(color2 >> 0);

    uint8 a3 = (uint8)(color3 >> 24);
    uint8 r3 = (uint8)(color3 >> 16);
    uint8 g3 = (uint8)(color3 >> 8);
    uint8 b3 = (uint8)(color3 >> 0);

    uint8 final_a = (uint8)(a1 * lam_1 + a2 * lam_2 + a3 * lam_3);
    uint8 final_r = (uint8)(r1 * lam_1 + r2 * lam_2 + r3 * lam_3);
    uint8 final_g = (uint8)(g1 * lam_1 + g2 * lam_2 + g3 * lam_3);
    uint8 final_b = (uint8)(b1 * lam_1 + b2 * lam_2 + b3 * lam_3);

    // return ((final_a&0xff) << 24) + ((final_r&0xff) << 16) + ((final_g&0xff) << 8) + final_b&0xff; // <- DEBUG
    return (final_a << 24) + (final_r << 16) + (final_g << 8) + final_b;
}
void gameUpdateAndRender(BackBuffer back_buffer)
{
    clearScreen(back_buffer);
    Triangle2D triangle = {{200.0f, 200.0f}, {500.0f, 200.0f}, {200.0f, 500.0f}};
    // SECTION: rasterize triangle
    int32 scanline_x_start[1280] = {};
    Vec2f *vertices = (Vec2f *)&triangle;
    for (int i1 = 0; i1 < 3; i1++)
    {
        int i2 = (i1 + 1) % 3;
        Vec2f v1 = vertices[i1];
        Vec2f v2 = vertices[i2];
        Vec2f lower_vertex = v1;
        Vec2f higher_vertex = v2;
        if (v1.y > v2.y)
        {
            lower_vertex = v2;
            higher_vertex = v1;
        }
        for (real32 y = lower_vertex.y; y < higher_vertex.y; y++)
        {
            real32 relative_y_diff = y - lower_vertex.y;
            real32 lerp_unit = 1.0f / (higher_vertex.y - lower_vertex.y);
            real32 x_bound = lerp(lower_vertex.x, higher_vertex.x, lerp_unit * relative_y_diff);
            if (scanline_x_start[(int32)y])
            {
                // this row has scanline boundary cached for this triangle
                real32 lower_x_bound;
                real32 higher_x_bound;
                if (scanline_x_start[(int32)y] > x_bound)
                {
                    lower_x_bound = (real32)x_bound;
                    higher_x_bound = (real32)scanline_x_start[(int32)y];
                }
                else
                {
                    higher_x_bound = (real32)x_bound;
                    lower_x_bound = (real32)scanline_x_start[(int32)y];
                }
                for (real32 x = lower_x_bound; x < higher_x_bound; x++)
                {
                    back_buffer((int32)x, (int32)y) = interpolatedColor(triangle, x, y, 0xffff00ff, 0xffffff00, 0xffffffff);
                }
                scanline_x_start[(int32)y] = 0;
            }
            else
            {
                // this row doesn't have scanline boundary cached for this triangle
                scanline_x_start[(int32)y] = (int32)x_bound;
            }
        }
    }
    return;
};
internal void Win32ResizeDibSection(HWND window)
{
    static RECT prev_size;
    RECT window_coords;
    GetWindowRect(window, &window_coords);
    // uint32 height = prev_size.bottom - prev_size.top;
    uint32 width = prev_size.right - prev_size.left;
    constexpr real64 aspect_ratio = 16. / 9.;
    // FIXME: set the client rect to this size instead of the whole window
    SetWindowPos(window, HWND_NOTOPMOST, window_coords.left, window_coords.top, width, (int)((real64)width / aspect_ratio), 0);
    prev_size = window_coords;
}
internal int32 Win32UpdateWindow(HDC device_context, HWND window, BackBuffer back_buffer)
{
    // TODO: benchmark against https://gamedev.net/forums/topic/385918-fast-drawing-to-screen-win32gdi/3552067/
    RECT rect;
    GetClientRect(window, &rect);
    BITMAPINFO bitmap_info = {};
    bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmap_info.bmiHeader.biWidth = 1280;
    bitmap_info.bmiHeader.biHeight = 720;
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
int32 WINAPI WinMain(_In_ HINSTANCE hInstance, HINSTANCE, PSTR, int32)
{
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
    HDC device_context = GetDC(window);
    BackBuffer back_buffer = {};
    back_buffer.width = 1280;
    back_buffer.height = 720;
    memory_index DIB_size = sizeof(uint32) * 1280 * 720;
    back_buffer.bits = (uint32 *)VirtualAlloc(0, DIB_size, MEM_COMMIT, PAGE_READWRITE);
    SetWindowPos(window, HWND_TOP, 300, 180, back_buffer.width, back_buffer.height, 0); // FIXME: weird black stripes

    keyboard_state = {};
    Win32ResizeDibSection(window);
    const real64 target_fps = 60;
    const real64 ms_per_tick = 1000.0 / target_fps;
    uint64 ticks = 0;
    real64 last_tick = GetTimeMillis();
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
        gameUpdateAndRender(back_buffer);
        Win32UpdateWindow(device_context, window, back_buffer);
    }
    return 0;
}