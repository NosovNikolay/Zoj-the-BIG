#include "application.h"

void mx_render_animation(t_animation *anim, SDL_Renderer *renderer, SDL_Rect *dst) {
    SDL_Rect src = {anim->sheet_width * anim->curr_frame,
                    0,
                    anim->sheet_width,
                    anim->sheet_height};
    SDL_RenderCopy(renderer, anim->spritesheet, &src, dst);
}

void mx_handle_animation(t_animation *anim, Uint32 ticks) {
    if (anim->old_time + anim->time > ticks)
        return;
    anim->old_time = ticks;
    ++anim->curr_frame;
    if (anim->curr_frame >= anim->frames_count)
        anim->curr_frame = 0;
}
