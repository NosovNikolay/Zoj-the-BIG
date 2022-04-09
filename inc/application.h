#pragma once
#include <stdbool.h>
#include <stdlib.h>

#include "endgame.h"
#include "pig.h"
#include "user_save.h"
#include "animation.h"

#define MX_WIDTH 1920
#define MX_HEIGHT 1080

typedef struct s_context t_context;

typedef struct s_state_menu {
    SDL_Texture *logo;
    SDL_Texture *apple;
    SDL_Texture *exit_button;
    SDL_Texture *exit_button_selected;
    Uint32 apple_timer;
    int levels_count;
    int current_level;
    int last_visited_level;
    SDL_Texture **levels;
    SDL_Texture **selected_levels;
    SDL_Texture **unable_levels;
    Mix_Chunk *switch_sound;
    void (*input)(t_context *ctx);
    void (*update)(t_context *ctx);
    void (*render)(t_context *ctx);
    void (*cleanup)(t_context *ctx);
} t_state_menu;

typedef struct s_state_game {
    int index;
    t_animation curr_anim;
    SDL_Texture *pig_skins[12];  // 4 textures * 3 weights.
    bool ctrls_locked;
    bool button_pressed;
    int weight;
    int wlevel;
    Uint32 animation_timer;
    SDL_Texture *apple_skin;
    SDL_Texture *tile;
    SDL_Texture *wall;
    SDL_Texture *pasby;
    SDL_Texture *button;
    SDL_Texture *b_button;
    SDL_Texture *up;
    SDL_Texture *down;
    SDL_Texture *left;
    SDL_Texture *right;
    Mix_Chunk *eat_sound;
    Mix_Chunk *step_sound;
    Mix_Chunk *pipe_sound;
    t_pig *pig;
    t_map *map;
    int apple_count;
    void (*input)(t_context *ctx);
    void (*update)(t_context *ctx);
    void (*render)(t_context *ctx);
    void (*cleanup)(t_context *ctx);
    char direction; 
    t_point *to;
} t_state_game;

typedef enum e_state_type {
    ST_MENU,
    ST_GAME,
} t_state_type;

struct s_context {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;
    SDL_DisplayMode display;
    Uint32 delta_time;
    bool is_running;
    t_state_type state_type;
    t_state_menu *menu;
    t_state_game *game;
    t_user_save *save;
    int current_level;
    int max_available_level;
    int levels_count;
    Mix_Music *bg_music;
};

void mx_run_main_loop(void);
