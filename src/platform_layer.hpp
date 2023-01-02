#pragma once
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
struct Mouse_Info{
    bool held_down;
    int x;
    int y;
};

enum GAMEPAD_RUMBLE_LEVEL
{
    OFF,
    LEVEL_0 = 7000,
    LEVEL_1 = 16000,
    LEVEL_2 = 30000,
    LEVEL_3 = 65535
};

struct argb_texture
{
    uint_32 *bits;
    uint_32 width;
    uint_32 height;
};

typedef size_t memory_index;

struct file_contents
{
    uint_64 size;
    void *data;
    static file_contents readWholeFile(char *path, uint_64 min_allocd_size = 0);
    void free();
};
void TerminateProcess(int ret_code);