#pragma once
#include "endgame.h"

typedef struct s_animation {
    SDL_Texture *spritesheet;
    int sheet_width;
    int sheet_height;
    int frames_count;
    int curr_frame;
    Uint32 time;
    Uint32 old_time;
}              t_animation;

void mx_render_animation(t_animation *anim, SDL_Renderer *renderer, SDL_Rect *dst);
void mx_handle_animation(t_animation *anim, Uint32 ticks);
