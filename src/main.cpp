
#include "common_defines.h"
#include "main.h"
void game_update_and_render(BACK_BUFFER back_buffer)
{
    for (int i = 0; i < back_buffer.height; i++)
    {
        for (int ii = 0; ii < back_buffer.width; ii++)
        {
            back_buffer.bits[ii + i * scene_width] = 0xffaaff;
        }
    }
    return;
}