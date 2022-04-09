#pragma once

#include <dirent.h>

#include "application.h"
#include "state_game.h"

#define MX_APPLES_COUNT 4
#define MX_TIME_TO_MOVE 2000.f

#define MX_MENU_LOGO_PATH "resource/assets/logo.png"
#define MX_MENU_APPLE_PATH "resource/assets/Apple.png"
#define MX_MENU_LEVELS_PATH "resource/levels"
#define MX_MENU_LEVEL_ASSET_PATH_PATTERN "resource/assets/lvl%d.png"
#define MX_MENU_LEVEL_SELECTED_ASSET_PATH_PATTERN "resource/assets/lvl_%d.png"
#define MX_MENU_LEVEL_UNABLE_ASSET_PATH_PATTERN "resource/assets/lock_lvl%d.png"
#define MX_MENU_EXIT_PATH "resource/assets/ext.png"
#define MX_MENU_EXIT_SELECTED_PATH "resource/assets/ext_.png"

#define MX_SWITCH_SOUND "resource/sounds/Button.wav"

t_state_menu *mx_create_state_menu(t_context *ctx);
