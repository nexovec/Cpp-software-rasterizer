#pragma once
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
struct GamepadInputs
{
    bool BUTTON_A;
    bool BUTTON_B;
    bool BUTTON_X;
    bool BUTTON_Y;
    bool DPAD_LEFT;
    bool DPAD_RIGHT;
    bool DPAD_UP;
    bool DPAD_DOWN;
    bool LEFT_SHOULDER;
    bool RIGHT_SHOULDER;
    bool LEFT_THUMB;
    bool RIGHT_THUMB;
    bool START;
    bool BACK;
};
struct KeyboardState
{
    bool KEY_A;
    bool KEY_S;
    bool KEY_D;
    bool KEY_W;
    bool KEY_SPACE;
};
enum GAMEPAD_RUMBLE_LEVEL
{
    OFF,
    LEVEL_0 = 7000,
    LEVEL_1 = 16000,
    LEVEL_2 = 30000,
    LEVEL_3 = 65535
};

struct BackBuffer
{
    uint32 *bits;
    uint32 width;
    uint32 height;
};

typedef size_t memory_index;

typedef float real32;
typedef double real64;