#pragma once

#include "application.h"
#include "map.h"

char mx_get_point(t_point *p, t_map* m);

bool mx_is_valid_point(t_point *p, t_map* map);

bool mx_can_move_pipe(char p, char d);

bool mx_can_move(t_point *dst, t_point *from, t_map* map, char direction, int wlevel);

bool mx_is_point_equal(t_point *p1, t_point *p2);

bool mx_is_trigger(t_point *point, t_map *map);

t_trigger *mx_get_trigger_trigger(t_map *map, t_point *point);

void mx_pipe_rotate(t_map *map, t_point *p);

void mx_resolve_trigger(t_context *ctx);

int mx_get_pig_weight_level(int weight);
