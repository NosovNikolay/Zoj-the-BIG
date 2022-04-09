#pragma once

#define MX_EMPTY '.'
#define MX_BLOCK '#'
#define MX_BUTTON 'b'
#define MX_B_BUTTON 'B'
#define MX_APPLE 'a'
#define MX_PORTAL 'p'
#define MX_PASBY '@'
#define MX_START '+'
#define MX_DIRECTION_LEFT 'l'
#define MX_DIRECTION_RIGHT 'r'
#define MX_DIRECTION_UP 'u'
#define MX_DIRECTION_DOWN 'd'

typedef struct s_point {
    int x;
    int y;
} t_point;

typedef struct s_trigger {
    t_point *point;
    char type;
    t_point **children;
    int child_count;
} t_trigger;

typedef struct s_map {
    int h;
    int w;
    char **array;
    t_trigger **triggers;
    int triggers_count;
} t_map;
