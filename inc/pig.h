#pragma once

#include <stdbool.h>

#include "map.h"

typedef struct s_pig {
    t_point *position;
    int weigth;
    int moves_count; // will need to indicate when we can reduce weight
    bool is_moving;
} t_pig;
