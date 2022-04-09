#include "state_game.h"
#include "animation.h"
#include "map.h"
#include "logic.h"

void mx_select_level(t_context *ctx, int index) {
    ctx->delta_time = 0;
    ctx->current_level = index;
    ctx->game = mx_create_state_game(ctx);
    ctx->state_type = ST_GAME;
}

t_point *mx_copy_point(t_point *p) {
    t_point *copy = (t_point *)malloc(sizeof(t_point));
    copy->x = p->x;
    copy->y = p->y;
    return copy;
}
// <= pay attention
static void mx_free_map(t_map *map) {
    for (int i = 0; i <= map->h; i++) {
        free(map->array[i]);
    }
    free(map->array);
    map->array = NULL;
    for (int i = 0; i < map->triggers_count; i++) {
        free(map->triggers[i]);
    }
    free(map->triggers);
    map->triggers = NULL;
}   

static void mx_input_game(t_context *ctx) {
    if (ctx->event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
        mx_exit_level(ctx);
        return;
    }
    if (ctx->event.key.keysym.scancode == SDL_SCANCODE_R) {
        mx_restart_level(ctx);
        return;
    }
    if (ctx->event.type != SDL_KEYDOWN || ctx->game->ctrls_locked) return;
    SDL_Texture *cur_tex = ctx->game->curr_anim.spritesheet;
    t_point *to = mx_copy_point(ctx->game->pig->position);
    char direction = '\0';
    switch (ctx->event.key.keysym.scancode) {
        case SDL_SCANCODE_DOWN:
        case SDL_SCANCODE_S:
            direction = MX_DIRECTION_DOWN;
            to->y++;
            if (ctx->game->direction != direction)
                ctx->game->curr_anim.spritesheet = ctx->game->pig_skins[ctx->game->wlevel * 4 + MX_DOWN_OFF];
            break;
        case SDL_SCANCODE_UP:
        case SDL_SCANCODE_W:
            direction = MX_DIRECTION_UP;
            to->y--;
            if (ctx->game->direction != direction)
                ctx->game->curr_anim.spritesheet = ctx->game->pig_skins[ctx->game->wlevel * 4 + MX_UP_OFF];
            break;
        case SDL_SCANCODE_LEFT:
        case SDL_SCANCODE_A:
            direction = MX_DIRECTION_LEFT;
            to->x--;
            if (ctx->game->direction != direction)
                ctx->game->curr_anim.spritesheet = ctx->game->pig_skins[ctx->game->wlevel * 4 + MX_LEFT_OFF];
            break;
        case SDL_SCANCODE_RIGHT:
        case SDL_SCANCODE_D:
            direction = MX_DIRECTION_RIGHT;
            to->x++;
            if (ctx->game->direction != direction)
                ctx->game->curr_anim.spritesheet = ctx->game->pig_skins[ctx->game->wlevel * 4 + MX_RIGHT_OFF];
            break;
        default:
            free(to);
            return;
    }

    char cur_point = mx_get_point(ctx->game->pig->position, ctx->game->map);
    if (cur_point == MX_PASBY &&
        (direction == MX_DIRECTION_LEFT || direction == MX_DIRECTION_RIGHT)) {
        ctx->game->curr_anim.spritesheet = cur_tex;
        ctx->game->curr_anim.curr_frame = 0;
        free(to);
        return;
    }

    if (!mx_can_move(to, ctx->game->pig->position, ctx->game->map, direction, ctx->game->wlevel)) {
        ctx->game->curr_anim.curr_frame = 0;
        ctx->game->direction = direction;
        free(to);
        return;
    }

    ctx->game->direction = direction;
    ctx->game->to = to;
    ctx->game->ctrls_locked = true;
}

static void mx_update_game(t_context *ctx) {
    char point = mx_get_point(ctx->game->pig->position, ctx->game->map);
    if (point == MX_APPLE) {
        ctx->game->weight += 200;
        ctx->game->map
            ->array[ctx->game->pig->position->y][ctx->game->pig->position->x] =
            MX_EMPTY;
        Mix_PlayChannel(1, ctx->game->eat_sound, 0);
        ctx->game->apple_count--;
    }
    if (!ctx->game->apple_count) {
        mx_finish_level(ctx);
        return;
    }
    if (!ctx->game->button_pressed &&
        mx_is_trigger(ctx->game->pig->position, ctx->game->map)) {
        mx_resolve_trigger(ctx);
        ctx->game->button_pressed = true;
    }

    if (point == MX_DIRECTION_RIGHT)
        ctx->game->curr_anim.spritesheet = ctx->game->pig_skins[ctx->game->wlevel * 4 + MX_RIGHT_OFF];
    if (point == MX_DIRECTION_LEFT)
        ctx->game->curr_anim.spritesheet = ctx->game->pig_skins[ctx->game->wlevel * 4 + MX_LEFT_OFF];
    if (point == MX_DIRECTION_UP) 
        ctx->game->curr_anim.spritesheet = ctx->game->pig_skins[ctx->game->wlevel * 4 + MX_UP_OFF];
    if (point == MX_DIRECTION_DOWN)
        ctx->game->curr_anim.spritesheet = ctx->game->pig_skins[ctx->game->wlevel * 4 + MX_DOWN_OFF];

    if (ctx->game->ctrls_locked) {
        ctx->game->animation_timer += ctx->delta_time;
        int frame = ctx->game->curr_anim.curr_frame;
        mx_handle_animation(&ctx->game->curr_anim, SDL_GetTicks());
        if (frame != ctx->game->curr_anim.curr_frame)
            Mix_PlayChannel(2, ctx->game->step_sound, 0);
        if (ctx->game->animation_timer > MX_MOVE_TIME) {
            ctx->game->animation_timer = 0;
            ctx->game->ctrls_locked = false;
        }
    }

    if (ctx->game->to == NULL ||
        (ctx->game->to != NULL &&
         mx_is_point_equal(ctx->game->to, ctx->game->pig->position)))
        ctx->game->curr_anim.curr_frame = 0;
    if (!ctx->game->ctrls_locked && ctx->game->to) {
        if (!mx_is_point_equal(ctx->game->to, ctx->game->pig->position)) {
            int frame = ctx->game->curr_anim.curr_frame;
            mx_handle_animation(&ctx->game->curr_anim, SDL_GetTicks());
            if (frame != ctx->game->curr_anim.curr_frame)
                Mix_PlayChannel(2, ctx->game->step_sound, 0);
        }
        ctx->game->button_pressed = false;
        ctx->game->pig->position->x = ctx->game->to->x;
        ctx->game->pig->position->y = ctx->game->to->y;
        if (ctx->game->weight > 100) ctx->game->weight -= 100;
        free(ctx->game->to);
        ctx->game->to = NULL;
    }
    ctx->game->wlevel = mx_get_pig_weight_level(ctx->game->weight);
}

static void mx_render_game(t_context *ctx) {
    int dw = ctx->display.w;
    int dh = ctx->display.h;

    SDL_SetRenderDrawColor(ctx->renderer, 90, 50, 0, 255);
    SDL_RenderClear(ctx->renderer);

    SDL_Rect map_rect = {0, 0, 0, 0};
    t_map *map = ctx->game->map;
    double tile_size = dw / map->w;
    map_rect.w = map->w * tile_size;
    map_rect.h = map->h * tile_size;
    if (map_rect.h > dh) {
        tile_size = dh / map->h;
        map_rect.w = map->w * tile_size;
        map_rect.h = map->h * tile_size;
        map_rect.x = (dw - map_rect.w) / 2;
    } else
        map_rect.y = (dh - map_rect.h) / 2;

    for (int i = 0; i < map->h; ++i) {
        for (int j = 0; j < map->w; ++j) {
            char tile = ctx->game->map->array[i][j];
            SDL_Rect tile_rect = {map_rect.x + j * tile_size,
                                  map_rect.y + i * tile_size, tile_size,
                                  tile_size};
            if (tile == MX_BLOCK)
                SDL_RenderCopy(ctx->renderer, ctx->game->wall, NULL,
                               &tile_rect);
            else {
                SDL_RenderCopy(ctx->renderer, ctx->game->tile, NULL,
                               &tile_rect);

                if (tile == MX_APPLE)
                    SDL_RenderCopy(ctx->renderer, ctx->game->apple_skin, NULL,
                                   &tile_rect);
                else if (tile == MX_PASBY)
                    SDL_RenderCopy(ctx->renderer, ctx->game->pasby, NULL,
                                   &tile_rect);
                else if (tile == MX_BUTTON)
                    SDL_RenderCopy(ctx->renderer, ctx->game->button, NULL,
                                   &tile_rect);
                else if (tile == MX_B_BUTTON)
                    SDL_RenderCopy(ctx->renderer, ctx->game->b_button, NULL,
                                   &tile_rect);
                else if (tile == MX_DIRECTION_UP)
                    SDL_RenderCopy(ctx->renderer, ctx->game->up, NULL,
                                   &tile_rect);
                else if (tile == MX_DIRECTION_DOWN)
                    SDL_RenderCopy(ctx->renderer, ctx->game->down, NULL,
                                   &tile_rect);
                else if (tile == MX_DIRECTION_LEFT)
                    SDL_RenderCopy(ctx->renderer, ctx->game->left, NULL,
                                   &tile_rect);
                else if (tile == MX_DIRECTION_RIGHT)
                    SDL_RenderCopy(ctx->renderer, ctx->game->right, NULL,
                                   &tile_rect);
            }
        }
    }

    if (ctx->game->ctrls_locked == true) {
        SDL_Rect pos = {map_rect.x + ctx->game->pig->position->x * tile_size,
                        map_rect.y + ctx->game->pig->position->y * tile_size,
                        tile_size, tile_size};
        pos.x -= (ctx->game->pig->position->x - ctx->game->to->x) * tile_size *
                 ((double)ctx->game->animation_timer / MX_MOVE_TIME);
        pos.y -= (ctx->game->pig->position->y - ctx->game->to->y) * tile_size *
                 ((double)ctx->game->animation_timer / MX_MOVE_TIME);
        mx_render_animation(&ctx->game->curr_anim, ctx->renderer, &pos);
    } else {
        mx_render_animation(
            &ctx->game->curr_anim, ctx->renderer,
            &(SDL_Rect){map_rect.x + ctx->game->pig->position->x * tile_size,
                        map_rect.y + ctx->game->pig->position->y * tile_size,
                        tile_size, tile_size});
    }

    SDL_RenderPresent(ctx->renderer);
}

static void mx_cleanup_game(t_context *ctx) {
    for (int i = 0; i < MX_PIG_WEIGHTS_COUNT * 4; ++i)
        SDL_DestroyTexture(ctx->game->pig_skins[i]);

    Mix_FreeChunk(ctx->game->eat_sound);
    Mix_FreeChunk(ctx->game->step_sound);
    Mix_FreeChunk(ctx->game->pipe_sound);

    if (ctx->game->apple_count) SDL_DestroyTexture(ctx->game->apple_skin);
    if (ctx->game->pasby) SDL_DestroyTexture(ctx->game->pasby);
    if (ctx->game->tile) SDL_DestroyTexture(ctx->game->tile);
    if (ctx->game->wall) SDL_DestroyTexture(ctx->game->wall);
    if (ctx->game->up) SDL_DestroyTexture(ctx->game->up);
    if (ctx->game->down) SDL_DestroyTexture(ctx->game->down);
    if (ctx->game->left) SDL_DestroyTexture(ctx->game->left);
    if (ctx->game->right) SDL_DestroyTexture(ctx->game->right);
    if (ctx->game->to) free(ctx->game->to);
    free(ctx->game->pig->position);
    free(ctx->game->pig);
    ctx->game->direction = '\0';
    mx_free_map(ctx->game->map);
}

static t_point *mx_get_pig_start(t_map *map) {
    t_point *p = (t_point *)malloc(sizeof(t_point));
    for (p->y = 0; p->y < map->h; p->y++)
        for (p->x = 0; p->x < map->w; p->x++)
            if (map->array[p->y][p->x] == MX_START) return p;
    return p;
}

static int mx_count_apples(t_map *map) {
    int count = 0;
    for (int y = 0; y < map->h; y++)
        for (int x = 0; x < map->w; x++)
            if (map->array[y][x] == MX_APPLE) count++;
    return count;
}

t_state_game *mx_create_state_game(t_context *ctx) {
    t_state_game *state = malloc(sizeof(t_state_game));
    if (state == NULL) return NULL;
    state->input = mx_input_game;
    state->update = mx_update_game;
    state->render = mx_render_game;
    state->cleanup = mx_cleanup_game;

    // Map loading
    state->index = ctx->current_level;
    state->pig = malloc(sizeof(t_pig));
    char map_path[64];
    sprintf(map_path, MX_LEVEL_MAP_PATH_PATTERN, ctx->current_level);
    char meta_path[64];
    sprintf(meta_path, MX_LEVEL_META_PATH_PATTERN, ctx->current_level);
    state->map = mx_level_parser(map_path, meta_path);

    // Animation loading
    char path[64];
    for (int i = 0; i < MX_PIG_WEIGHTS_COUNT; ++i) {
        sprintf(path, MX_PIG_UP_TEXTURE_PATTERN, state->index, i + 1);
        state->pig_skins[4 * i + MX_UP_OFF] = IMG_LoadTexture(ctx->renderer, path);
        sprintf(path, MX_PIG_DOWN_TEXTURE_PATTERN, state->index, i + 1);
        state->pig_skins[4 * i + MX_DOWN_OFF] = IMG_LoadTexture(ctx->renderer, path);
        sprintf(path, MX_PIG_LEFT_TEXTURE_PATTERN, state->index, i + 1);
        state->pig_skins[4 * i + MX_LEFT_OFF] = IMG_LoadTexture(ctx->renderer, path);
        sprintf(path, MX_PIG_RIGHT_TEXTURE_PATTERN, state->index, i + 1);
        state->pig_skins[4 * i + MX_RIGHT_OFF] = IMG_LoadTexture(ctx->renderer, path);
    }
    state->ctrls_locked = false;
    state->animation_timer = 0;
    state->weight = 900;
    state->wlevel = 0;
    state->curr_anim = (t_animation){state->pig_skins[state->wlevel * 4 + MX_DOWN_OFF], 64, 64, 3, 0, 150, 0};

    // Texture loading
    sprintf(path, MX_TILE_TEXTURE_PATH_PATTERN, state->index);
    state->tile = IMG_LoadTexture(ctx->renderer, path);
    sprintf(path, MX_WALL_TEXTURE_PATH_PATTERN, state->index);
    state->wall = IMG_LoadTexture(ctx->renderer, path);
    sprintf(path, MX_PASBY_TEXTURE_PATH_PATTERN, state->index);
    state->pasby = IMG_LoadTexture(ctx->renderer, path);
    sprintf(path, MX_BUTTON_TEXTURE_PATH_PATTERN, state->index);
    state->button = IMG_LoadTexture(ctx->renderer, path);
    sprintf(path, MX_B_BUTTON_TEXTURE_PATH_PATTERN, state->index);
    state->b_button = IMG_LoadTexture(ctx->renderer, path);
    sprintf(path, MX_LEVEL_APPLE_PATH_PATTERN, state->index);
    state->apple_skin = IMG_LoadTexture(ctx->renderer, path);

    sprintf(path, MX_UP_TEXTURE_PATH_PATTERN, state->index);
    state->up = IMG_LoadTexture(ctx->renderer, path);
    sprintf(path, MX_DOWN_TEXTURE_PATH_PATTERN, state->index);
    state->down = IMG_LoadTexture(ctx->renderer, path);
    sprintf(path, MX_LEFT_TEXTURE_PATH_PATTERN, state->index);
    state->left = IMG_LoadTexture(ctx->renderer, path);
    sprintf(path, MX_RIGHT_TEXTURE_PATH_PATTERN, state->index);
    state->right = IMG_LoadTexture(ctx->renderer, path);
    // Audio
    state->eat_sound = Mix_LoadWAV(MX_EAT_SOUND);
    state->step_sound = Mix_LoadWAV(MX_STEP_SOUND);
    state->pipe_sound = Mix_LoadWAV(MX_PIPE_SOUND);

    // Level info
    state->button_pressed = false;
    state->pig->position = mx_get_pig_start(state->map);
    state->apple_count = mx_count_apples(state->map);
    state->direction = 0;
    state->to = NULL;
    return state;
}

void mx_restart_level(t_context *ctx) {
    int index = ctx->game->index;
    ctx->game->cleanup(ctx);
    mx_select_level(ctx, index);
}

void mx_exit_level(t_context *ctx) {
    ctx->state_type = ST_MENU;
    ctx->save->max_available_lvl = ctx->max_available_level;
    ctx->save->last_visited_lvl = ctx->current_level;
    mx_update_user_save(ctx->save);
}

void mx_finish_level(t_context *ctx) {
    if (ctx->game->index == ctx->levels_count) {
        ctx->current_level = 1;
        mx_exit_level(ctx);
    } else {
        int next_index = ctx->game->index + 1;
        ctx->current_level = next_index;
        if (ctx->max_available_level <= ctx->game->index)
            ctx->max_available_level = next_index;
        mx_update_user_save(ctx->save);
        ctx->game->cleanup(ctx);
        mx_select_level(ctx, next_index);
    }
}
