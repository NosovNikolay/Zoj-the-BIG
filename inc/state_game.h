#pragma once

#include "logic.h"
#include "parser.h"
#include "application.h"

#define MX_MOVE_TIME 750
#define MX_PIG_WEIGHTS_COUNT 3
#define MX_UP_OFF 1
#define MX_DOWN_OFF 0
#define MX_LEFT_OFF 2
#define MX_RIGHT_OFF 3

#define MX_PIG_UP_TEXTURE_PATTERN "resource/levels/%d/pig/%d/up.png"
#define MX_PIG_DOWN_TEXTURE_PATTERN "resource/levels/%d/pig/%d/down.png"
#define MX_PIG_LEFT_TEXTURE_PATTERN "resource/levels/%d/pig/%d/left.png"
#define MX_PIG_RIGHT_TEXTURE_PATTERN "resource/levels/%d/pig/%d/right.png"

#define MX_LEVEL_MAP_PATH_PATTERN "resource/levels/%d/map.txt"
#define MX_LEVEL_META_PATH_PATTERN "resource/levels/%d/meta.txt"
#define MX_LEVEL_TEXTURE_PATH_PATTERN "resource/levels/%d/level.png"
#define MX_LEVEL_APPLE_PATH_PATTERN "resource/levels/%d/apple.png"

#define MX_TILE_TEXTURE_PATH_PATTERN "resource/levels/%d/tile.png"
#define MX_WALL_TEXTURE_PATH_PATTERN "resource/levels/%d/wall.png"
#define MX_PASBY_TEXTURE_PATH_PATTERN "resource/levels/%d/pasby.png"
#define MX_BUTTON_TEXTURE_PATH_PATTERN "resource/levels/%d/button.png"
#define MX_B_BUTTON_TEXTURE_PATH_PATTERN "resource/levels/%d/b_button.png"

#define MX_UP_TEXTURE_PATH_PATTERN "resource/levels/%d/u.png"
#define MX_DOWN_TEXTURE_PATH_PATTERN "resource/levels/%d/d.png"
#define MX_LEFT_TEXTURE_PATH_PATTERN "resource/levels/%d/l.png"
#define MX_RIGHT_TEXTURE_PATH_PATTERN "resource/levels/%d/r.png"

#define MX_EAT_SOUND "resource/sounds/Eat.wav"
#define MX_STEP_SOUND "resource/sounds/Step.wav"
#define MX_PIPE_SOUND "resource/sounds/Pipe.wav"

void mx_select_level(t_context *ctx, int index);

t_point *mx_copy_point(t_point *p);

t_state_game *mx_create_state_game(t_context *ctx);

void mx_restart_level(t_context *ctx);

void mx_exit_level(t_context *ctx);

void mx_finish_level(t_context *ctx);
