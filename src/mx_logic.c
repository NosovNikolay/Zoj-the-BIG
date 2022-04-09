#include "logic.h"
#include "state_game.h"

char mx_get_point(t_point *p, t_map *m) { return m->array[p->y][p->x]; }

bool mx_is_valid_point(t_point *p, t_map *map) {
    return p->x >= 0 && p->x < map->w && p->y >= 0 && p->y < map->h;
}

bool mx_can_move_pipe(char p, char d) {
    if (p == d) return true;
    return p != MX_DIRECTION_UP && p != MX_DIRECTION_DOWN &&
           p != MX_DIRECTION_RIGHT && p != MX_DIRECTION_LEFT;
}

bool mx_can_move(t_point *dst, t_point *from, t_map *map, char direction, int wlevel) {
    if (!mx_is_valid_point(dst, map)) return false;
    if (!mx_can_move_pipe(mx_get_point(from, map), direction)) return false;

    char dst_val = mx_get_point(dst, map);

    if (dst_val == MX_BLOCK) return false;

    if (dst_val == MX_PASBY) return wlevel == 0;

    switch (dst_val) {
        case MX_DIRECTION_LEFT:
            return direction != MX_DIRECTION_RIGHT;
        case MX_DIRECTION_RIGHT:
            return direction != MX_DIRECTION_LEFT;
        case MX_DIRECTION_UP:
            return direction != MX_DIRECTION_DOWN;
        case MX_DIRECTION_DOWN:
            return direction != MX_DIRECTION_UP;
        default:
            break;
    }

    return true;
}

void mx_pipe_rotate(t_map *map, t_point *p) {
    char tmp = mx_get_point(p, map);
    switch (tmp) {
        case MX_DIRECTION_DOWN:
            map->array[p->y][p->x] = MX_DIRECTION_LEFT;
            break;
        case MX_DIRECTION_LEFT:
            map->array[p->y][p->x] = MX_DIRECTION_UP;
            break;
        case MX_DIRECTION_UP:
            map->array[p->y][p->x] = MX_DIRECTION_RIGHT;
            break;
        case MX_DIRECTION_RIGHT:
            map->array[p->y][p->x] = MX_DIRECTION_DOWN;
            break;
        default:
            break;
    }
}

bool mx_is_point_equal(t_point *p1, t_point *p2) {
    return p1->x == p2->x && p1->y == p2->y;
}

bool mx_is_trigger(t_point *point, t_map *map) {
    char tmp = mx_get_point(point, map);
    return tmp == MX_B_BUTTON || tmp == MX_BUTTON || tmp == MX_PORTAL;
}

t_trigger *mx_get_trigger_trigger(t_map *map, t_point *point) {
    for (int i = 0; i < map->triggers_count; i++) {
        if (mx_is_point_equal(map->triggers[i]->point, point)) {
            return map->triggers[i];
        }
    }
    return NULL;
}

void mx_resolve_trigger(t_context *ctx) {
    t_trigger *tmp =
        mx_get_trigger_trigger(ctx->game->map, ctx->game->pig->position);
    char type = mx_get_point(ctx->game->pig->position, ctx->game->map);
    if (!tmp) return;
    if(type == MX_B_BUTTON && ctx->game->wlevel < 1) return;

    switch (type) {
        case MX_BUTTON:
        case MX_B_BUTTON:
            for (int i = 0; i < tmp->child_count; i++) {
                mx_pipe_rotate(ctx->game->map, tmp->children[i]);
                Mix_PlayChannel(3, ctx->game->pipe_sound, 0);
            }
            break;
        case MX_PORTAL:
            free(ctx->game->pig->position);
            ctx->game->pig->position = mx_copy_point(tmp->children[0]);
            break;
        default:
            break;
    }
}

int mx_get_pig_weight_level(int weight) {
    if (weight > 1500) return 2;
    if (weight > 700) return 1;
    return 0;
}
