#include "win32_platform.h"
#include "common_defines.h"
#include "main.h"
#include <windows.h>
#include <xinput.h>

// FIXME: shame on you
#include <stdio.h>

global BOOL running = true;
global KeyboardState keyboard_state;

#define XINPUT_GET_STATE_SIG(name) uint64 name(uint64 dwUserIndex, XINPUT_STATE *pState)
typedef XINPUT_GET_STATE_SIG(x_input_get_state);
XINPUT_GET_STATE_SIG(XInputGetStateStub)
{
    return 0;
}
global x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

#define XINPUT_SET_STATE_SIG(name) uint64 name(uint64 dwUserIndex, XINPUT_VIBRATION *pVibration)
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
    for (int32 i = 0; i < registered_controllers; i++)
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
    // uint32 height = prev_size.bottom - prev_size.top;
    uint32 width = prev_size.right - prev_size.left;
    constexpr real64 aspect_ratio = 16. / 9.;
    // FIXME: set the client rect to this size instead of the whole window
    SetWindowPos(window, HWND_NOTOPMOST, window_coords.left, window_coords.top, width, (int)((real64)width / aspect_ratio), 0);
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
    int32 width = rect.right - rect.left;
    int32 height = rect.bottom - rect.top;
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
        ExitProcess(-1);
    }
    persistent LARGE_INTEGER lpFrequency;
    if (!QueryPerformanceFrequency(&lpFrequency))
    {
        // TODO: error handle
        ExitProcess(-1);
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
    int64 size;
    void *data;
    static file_contents readWholeFile(char *path);
    void free();
};
void file_contents::free()
{
    VirtualFree(this->data, 0, MEM_RELEASE);
}
file_contents file_contents::readWholeFile(char *path)
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
    else if (!ReadFile(file_handle, file.data, (uint64)file.size, &bytes_read, 0))
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
    sprintf((char *const)&print, "The page size for this system is %u bytes.\n", si.dwPageSize);
    OutputDebugStringA(print);
#endif
}
#pragma pack(push, 1)
struct BitmapHeader1
{
    uint16 Type;       /* File type identifier (always 0) */
    uint16 Width;      /* Width of the bitmap in pixels */
    uint16 Height;     /* Height of the bitmap in scan lines */
    uint16 ByteWidth;  /* Width of bitmap in ints */
    int8 Planes;       /* Number of color planes */
    int8 BitsPerPixel; /* Number of bits per pixel */
};
struct BitmapHeader
{
    uint16 FileType;        /* File type, always 4D42h ("BM") */
    uint64 FileSize;        /* Size of the file in bytes */
    uint16 Reserved1;       /* Always 0 */
    uint16 Reserved2;       /* Always 0 */
    uint64 BitmapOffset;    /* Starting position of image data in bytes */
    uint64 Size;            /* Size of this header in bytes */
    int64 Width;            /* Image width in pixels */
    int64 Height;           /* Image height in pixels */
    uint16 Planes;          /* Number of color planes */
    uint16 BitsPerPixel;    /* Number of bits per pixel */
    uint64 Compression;     /* Compression methods used */
    uint64 SizeOfBitmap;    /* Size of bitmap in bytes */
    int64 HorzResolution;   /* Horizontal resolution in pixels per meter */
    int64 VertResolution;   /* Vertical resolution in pixels per meter */
    uint64 ColorsUsed;      /* Number of colors in the image */
    uint64 ColorsImportant; /* Minimum number of important colors */
};
struct BitmapHeader3
{
    uint64 Size;         /* Size of this header in bytes */
    int64 Width;         /* Image width in pixels */
    int64 Height;        /* Image height in pixels */
    uint16 Planes;       /* Number of color planes */
    uint16 BitsPerPixel; /* Number of bits per pixel */
    /* Fields added for Windows 3.x follow this line */
    uint64 Compression;     /* Compression methods used */
    uint64 SizeOfBitmap;    /* Size of bitmap in bytes */
    int64 HorzResolution;   /* Horizontal resolution in pixels per meter */
    int64 VertResolution;   /* Vertical resolution in pixels per meter */
    uint64 ColorsUsed;      /* Number of colors in the image */
    uint64 ColorsImportant; /* Minimum number of important colors */
};
struct BitmapHeader4
{
    uint64 Size;            /* Size of this header in bytes */
    int64 Width;            /* Image width in pixels */
    int64 Height;           /* Image height in pixels */
    uint16 Planes;          /* Number of color planes */
    uint16 BitsPerPixel;    /* Number of bits per pixel */
    uint64 Compression;     /* Compression methods used */
    uint64 SizeOfBitmap;    /* Size of bitmap in bytes */
    int64 HorzResolution;   /* Horizontal resolution in pixels per meter */
    int64 VertResolution;   /* Vertical resolution in pixels per meter */
    uint64 ColorsUsed;      /* Number of colors in the image */
    uint64 ColorsImportant; /* Minimum number of important colors */
    /* Fields added for Windows 4.x follow this line */

    uint64 RedMask;    /* Mask identifying bits of red component */
    uint64 GreenMask;  /* Mask identifying bits of green component */
    uint64 BlueMask;   /* Mask identifying bits of blue component */
    uint64 AlphaMask;  /* Mask identifying bits of alpha component */
    uint64 CSType;     /* Color space type */
    int64 RedX;        /* X coordinate of red endpoint */
    int64 RedY;        /* Y coordinate of red endpoint */
    int64 RedZ;        /* Z coordinate of red endpoint */
    int64 GreenX;      /* X coordinate of green endpoint */
    int64 GreenY;      /* Y coordinate of green endpoint */
    int64 GreenZ;      /* Z coordinate of green endpoint */
    int64 BlueX;       /* X coordinate of blue endpoint */
    int64 BlueY;       /* Y coordinate of blue endpoint */
    int64 BlueZ;       /* Z coordinate of blue endpoint */
    uint64 GammaRed;   /* Gamma red coordinate scale value */
    uint64 GammaGreen; /* Gamma green coordinate scale value */
    uint64 GammaBlue;  /* Gamma blue coordinate scale value */
};
#pragma pack(pop)
class uint32_2D_array_wrapper
{
    // ? TODO: Do we even want this whole thing?
    // FIXME: this is unsafe
    struct uint32_wrapper
    {
        uint32 *number;
        uint32_wrapper(uint32 *num) : number(num) {}
        uint32_wrapper operator*(uint32 other)
        {
            uint32 *number = this->number;
            *number *= other;
            return *this;
        }
        uint32_wrapper *operator=(uint32_wrapper other)
        {
            uint32 *number = this->number;
            *number = *other.number;
            return this;
        }
        uint32 operator==(uint32 other)
        {
            uint32 *number = this->number;
            return other == *number;
            // if (other == *number)
            //     return 1;
            // else
            //     return 0;
        }
    };
    struct row
    {
        uint32 *bits;
        row(uint32 *bits) : bits(bits) {}
        uint32_wrapper operator[](int32 y)
        {
            return uint32_wrapper(&this->bits[y]);
        }
    };
    uint32 *bits;
    uint32 width;

public:
    uint32_2D_array_wrapper(uint32 *bits, uint32 width)
    {
        this->bits = bits;
        this->width = width;
    }
    inline row operator[](int32 x)
    {
        return row(&(this->bits[this->width * x]));
    }
};
struct BitmapImage
{
    static BitmapImage loadBitmapFromFile(char *filepath);
    static BitmapImage setAlphaColor(BitmapImage bmp, uint32 alpha_color);
    BitmapHeader *bh;
    uint32 *pixels;
};
BitmapImage BitmapImage::setAlphaColor(BitmapImage bmp, uint32 alpha_color)
{
    uint32_2D_array_wrapper bmp_pixels = uint32_2D_array_wrapper(bmp.pixels, bmp.bh->Width);

    for (int32 x = 0; x < bmp.bh->Width; x++)
    {
        for (int32 y = 0; y < bmp.bh->Height; y++)
        {
            // zero out if alpha is 0
            // bmp_pixels[y][x] = bmp_pixels[y][x] * ((*(bmp_pixels[y][x].number) >> 24) & 0xff != 0);
            uint32 shouldZero = bmp_pixels[y][x] == alpha_color;
            bmp_pixels[y][x] = bmp_pixels[y][x] * !shouldZero;
        }
    }
    return bmp;
}
BitmapImage BitmapImage::loadBitmapFromFile(char *filepath)
{
    // FIXME: error handling
    BitmapImage bmp;
    bmp.bh = (BitmapHeader *)(file_contents::readWholeFile(filepath).data);
    bmp.pixels = (uint32 *)(bmp.bh + bmp.bh->BitmapOffset);
    return bmp;
}
struct Assets
{
    Assets();
    BitmapImage test_image;
    // ? FIXME: optional memory free
};
Assets::Assets()
{
    char *path = (char *)"font.bmp";
    // this->test_image = BitmapImage::loadBitmapFromFile(path);
    // ! FIXME:
    this->test_image = BitmapImage::setAlphaColor(BitmapImage::loadBitmapFromFile(path), 0xffff00ff);
    // FIXME: no safeguard against read errors
    return;
}
// void DEBUGBltBmp(BackBuffer &back_buffer, BitmapImage &bmp, int32 x_offset = 0, int32 y_offset = 0);
void DEBUGBltBmp(BackBuffer *back_buffer, BitmapImage *bmp, int32 x_offset, int32 y_offset)
{
    // FIXME: protect overflows
    // FIXME: colors are a bit broken
    uint32_2D_array_wrapper back_buffer_bits = uint32_2D_array_wrapper(back_buffer->bits, back_buffer->width);
    uint32_2D_array_wrapper bmp_pixels = uint32_2D_array_wrapper(bmp->pixels, bmp->bh->Width);

    for (int32 x = 0; x < bmp->bh->Width; x++)
    {
        for (int32 y = 0; y < bmp->bh->Height; y++)
        {
            back_buffer_bits[y + y_offset][x + x_offset] = bmp_pixels[y][x];
            // back_buffer_bits.set(x + x_offset, y + y_offset, bmp_pixels[x][y]);
            // back_buffer_bits.set(x + x_offset, y + y_offset, bmp_pixels.get(x, y));
        }
    }
}
int32 WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                     _In_ PSTR lpCmdLine, _In_ int32 nCmdShow)
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

    {
#ifdef DEBUG
        int32 h = assets.test_image.bh->Height;
        char buf[128];
        sprintf(buf, "%ld\n", h);
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
    memory_index DIB_size = sizeof(uint32) * default_scene_width * default_scene_height;
    back_buffer.bits = (uint32 *)VirtualAlloc(0, DIB_size, MEM_COMMIT, PAGE_READWRITE);
    SetWindowPos(window, HWND_TOP, 300, 180, back_buffer.width, back_buffer.height, 0); // FIXME: weird black stripes

    keyboard_state = {};
    Win32ResizeDibSection(window);
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
#ifndef DEBUG
        static_assert(false);
#endif
        DEBUGBltBmp(&back_buffer, &assets.test_image, 400, 300);

        //         if (!(GetTimeMillis() - last_tick < ms_per_tick * 2))
        //         {
        // // this game update went overbudget
        // // renders red screen
        // #define back_buffer(x, y) back_buffer.bits[back_buffer.width * y + x]
        // #define test_image(x, y) ((int32 *)assets.test_image.pixels)[assets.test_image.bh->Width * y + x]
        //             for (int32 i = 0; i < back_buffer.width * back_buffer.height; i++)
        //                 back_buffer.bits[i] &= 0xffff0000;
        //             // ? TODO: render "frozen" text in lower left corner instead (disconnect rendering from update??)
        //             // TODO: over a certain treshold, skip frame
        //             // TODO: skip based on average of last n ticks
        //             // TODO: measure skipped frames
        //         }
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
