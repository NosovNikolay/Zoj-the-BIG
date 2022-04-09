#include "state_menu.h"

static void mx_select_item(t_context* ctx) {
    if (ctx->menu->current_level == ctx->menu->levels_count + 1) {
        ctx->is_running = false;
        return;
    }
    mx_select_level(ctx, ctx->menu->current_level);
}

static void mx_input_menu(t_context* ctx) {
    if (ctx->event.type != SDL_KEYDOWN) return;

    switch (ctx->event.key.keysym.scancode) {
        case SDL_SCANCODE_DOWN:
        case SDL_SCANCODE_S:
            if (ctx->menu->current_level <= ctx->max_available_level - 1){
                ctx->menu->current_level++;
            }
            else if(ctx->menu->current_level == ctx->max_available_level ) {
                ctx->menu->current_level = ctx->menu->levels_count + 1;
            }
            else {
                ctx->menu->current_level = 1;
            }
        break;

        case SDL_SCANCODE_UP:
        case SDL_SCANCODE_W:
            if (ctx->menu->current_level == ctx->menu->levels_count + 1) {
                ctx->menu->current_level = ctx->max_available_level;
            }
            else if (ctx->menu->current_level > 1) {
                ctx->menu->current_level--;
            }
            else {
                ctx->menu->current_level = ctx->menu->levels_count + 1;
            }
        break;

        case SDL_SCANCODE_RETURN:
            mx_select_item(ctx);
            return;

        case SDL_SCANCODE_ESCAPE:
            ctx->is_running = false;
            return;

        default:
            return;
    }
    Mix_PlayChannel(1, ctx->menu->switch_sound, 0);
}

static void mx_update_menu(t_context* ctx) {
    ctx->menu->apple_timer += ctx->delta_time;
    if (ctx->menu->apple_timer > MX_TIME_TO_MOVE) ctx->menu->apple_timer = 0;
}

static int mx_get_lvls_count() {
    int max_level = 0;
    DIR* dir;
    struct dirent* ent;
    if ((dir = opendir(MX_MENU_LEVELS_PATH)) != NULL) {
        while ((ent = readdir(dir))) {
            int lvl_num = atoi(ent->d_name);
            if (lvl_num > max_level) max_level = lvl_num;
        }
        closedir(dir);
    }
    return max_level;
}

static void mx_render_levels(t_context* ctx) {
    double sph = ctx->display.h * 0.03;
    double spw = ctx->display.w * 0.03;
    int btnw;
    int btnh;
    SDL_QueryTexture(ctx->menu->levels[0], NULL, NULL, &btnw, &btnh);
    double scale = (double)btnw / (double)btnh;
    // Exit resolution
    int eh = ctx->display.h * 0.121;
    int ew = eh * scale;
    int exity = ctx->display.h - eh - ctx->display.h * 0.102;

    if (ctx->menu->levels_count > 0) {
        btnh = ctx->display.h * 0.061;
        btnw = btnh * scale;
        int ymin = ctx->display.h / 4 + ctx->display.h * 0.04;

        int y = ymin;
        int levels_per_col = (exity - y) / (double)(btnh + sph);
        int columns_num =
            SDL_ceil(ctx->menu->levels_count / (double)levels_per_col);
        int block_width = (columns_num * (btnw + spw)) - spw;
        int x = ctx->display.w / 2 - block_width / 2;
        int current_row = 0;
        SDL_Texture* texture = NULL;
        for (int i = 0; i < ctx->menu->levels_count; ++i) {
            if(i > ctx->max_available_level - 1) {
                texture = ctx->menu->unable_levels[i];
            }
            else if (ctx->menu->current_level == i + 1) {
                texture = ctx->menu->selected_levels[i];
            }
            else {
                texture = ctx->menu->levels[i];
            }

            SDL_RenderCopy(
                ctx->renderer, texture, NULL,
                &(SDL_Rect){x + ((btnw + spw) * current_row), y, btnw, btnh});
            y += (btnh + sph);
            if ((i + 1) / levels_per_col > current_row) {
                ++current_row;
                y = ymin;
            }
        }
    }
    SDL_Texture* exit_b_texture =
        ctx->menu->current_level == ctx->menu->levels_count + 1
            ? ctx->menu->exit_button_selected
            : ctx->menu->exit_button;
    SDL_RenderCopy(ctx->renderer, exit_b_texture, NULL,
                   &(SDL_Rect){ctx->display.w / 2 - ew / 2, exity, ew, eh});
}

static void mx_render_menu(t_context* ctx) {
    int dw = ctx->display.w;
    int dh = ctx->display.h;
    // Background color.
    SDL_SetRenderDrawColor(ctx->renderer, 174, 106, 245, 255);
    SDL_RenderClear(ctx->renderer);

    // Draw rect.
    SDL_Rect middle = {(dw - dw * 0.68) / 2, 0, dw * 0.68, dh};

    // Draw apples
    int appw = middle.x / MX_APPLES_COUNT;
    int apph = middle.x / MX_APPLES_COUNT;
    int offset = -appw * (ctx->menu->apple_timer / MX_TIME_TO_MOVE);
    for (int y = 0; y < dh; y += apph * 2) {
        for (int x = 0; x < MX_APPLES_COUNT + 1; ++x)
            SDL_RenderCopy(ctx->renderer, ctx->menu->apple, NULL,
                           &(SDL_Rect){x * appw + offset, y, appw, apph});
        for (int x = 0; x < MX_APPLES_COUNT + 1; ++x)
            SDL_RenderCopy(ctx->renderer, ctx->menu->apple, NULL,
                           &(SDL_Rect){middle.x + middle.w + x * appw + offset,
                                       y, appw, apph});
    }
    for (int y = apph; y < dh; y += apph * 2) {
        for (int x = -1; x < MX_APPLES_COUNT; ++x)
            SDL_RenderCopy(ctx->renderer, ctx->menu->apple, NULL,
                           &(SDL_Rect){x * appw - offset, y, appw, apph});
        for (int x = -1; x < MX_APPLES_COUNT; ++x)
            SDL_RenderCopy(ctx->renderer, ctx->menu->apple, NULL,
                           &(SDL_Rect){middle.x + middle.w + x * appw - offset,
                                       y, appw, apph});
    }

    SDL_SetRenderDrawColor(ctx->renderer, 113, 117, 238, 255);
    SDL_RenderFillRect(ctx->renderer, &middle);
    // Draw logo.
    int logow;
    int logoh;
    SDL_QueryTexture(ctx->menu->logo, NULL, NULL, &logow, &logoh);
    double scale = logow / logoh;
    logoh = dh / 4;
    logow = logoh * scale;
    SDL_RenderCopy(ctx->renderer, ctx->menu->logo, NULL,
                   &(SDL_Rect){dw / 2 - logow / 2, 0, logow, logoh});
    // Levels
    mx_render_levels(ctx);
    // End.
    SDL_RenderPresent(ctx->renderer);
}

static void mx_cleanup_menu(t_context* ctx) {
    SDL_DestroyTexture(ctx->menu->logo);
    SDL_DestroyTexture(ctx->menu->apple);
    SDL_DestroyTexture(ctx->menu->exit_button);
    SDL_DestroyTexture(ctx->menu->exit_button_selected);
    Mix_FreeChunk(ctx->menu->switch_sound);
    for (int i = 0; i < ctx->menu->levels_count; i++) {
        SDL_DestroyTexture(ctx->menu->levels[i]);
        SDL_DestroyTexture(ctx->menu->selected_levels[i]);
        SDL_DestroyTexture(ctx->menu->unable_levels[i]);
    }
    free(ctx->menu);
}

t_state_menu* mx_create_state_menu(t_context* ctx) {
    t_state_menu* state = malloc(sizeof(t_state_menu));
    if (state == NULL) return NULL;
    state->input = mx_input_menu;
    state->update = mx_update_menu;
    state->render = mx_render_menu;
    state->cleanup = mx_cleanup_menu;
    state->apple_timer = 0;
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    // ctx->menu uninitialized here. use only state->
    state->logo = IMG_LoadTexture(ctx->renderer, MX_MENU_LOGO_PATH);
    state->apple = IMG_LoadTexture(ctx->renderer, MX_MENU_APPLE_PATH);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    state->exit_button = IMG_LoadTexture(ctx->renderer, MX_MENU_EXIT_PATH);
    state->exit_button_selected =
        IMG_LoadTexture(ctx->renderer, MX_MENU_EXIT_SELECTED_PATH);
    state->levels_count = mx_get_lvls_count();
    ctx->levels_count = state->levels_count;
    state->current_level = 1;
    state->last_visited_level = ctx->current_level;
    state->levels = malloc(sizeof(SDL_Texture*) * state->levels_count);
    state->selected_levels = malloc(sizeof(SDL_Texture*) * state->levels_count);
    state->unable_levels = malloc(sizeof(SDL_Texture*) * state->levels_count);
    for (int i = 0; i < state->levels_count; i++) {
        char path[64];
        sprintf(path, MX_MENU_LEVEL_ASSET_PATH_PATTERN, i + 1);
        state->levels[i] = IMG_LoadTexture(ctx->renderer, path);
        sprintf(path, MX_MENU_LEVEL_SELECTED_ASSET_PATH_PATTERN, i + 1);
        state->selected_levels[i] = IMG_LoadTexture(ctx->renderer, path);
        sprintf(path, MX_MENU_LEVEL_UNABLE_ASSET_PATH_PATTERN, i + 1);
        state->unable_levels[i] = IMG_LoadTexture(ctx->renderer, path);
        
    }
    state->switch_sound = Mix_LoadWAV(MX_SWITCH_SOUND);
    return state;
}
