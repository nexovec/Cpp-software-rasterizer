#pragma once
struct GAMEPAD_INPUTS{
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
struct KEYBOARD_STATE{
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

struct BACK_BUFFER
{
    uint32_t *bits;
    uint16_t width;
    uint16_t height;
};