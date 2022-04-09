#include "parser.h"

static int filesize(const char *fname) {
    if (!mx_strlen(fname)) return -1;
    int size = 0;
    int fn = open(fname, O_RDONLY);
    if (fn == -1) return -1;
    char buff[1];
    while (read(fn, buff, 1)) size++;
    close(fn);
    return size;
}

char *mx_file_to_str(const char *filename) {
    if (!mx_strlen(filename)) return NULL;
    int size = filesize(filename);
    if (size < 0) return NULL;
    int fn = open(filename, O_RDONLY);
    char *text = (char *)malloc(size + 1);
    text[size] = '\0';
    read(fn, text, size);
    close(fn);
    return text;
}

char **mx_map_parser(const char *filename, int *w, int *h) {
    char *str = mx_file_to_str(filename);
    char **array = mx_strsplit(str, '\n', h);
    if (*h) 
        *w = mx_strlen(array[0]);
    return array;
}

t_point *mx_parse_point(char *coordinates) {
    int tmp;
    t_point *point = (t_point *)malloc(sizeof(t_point));

    char **xy = mx_strsplit(coordinates, ',', &tmp);
    point->x = atoi(xy[0]);
    point->y = atoi(xy[1]);
    return point;
}

// type:x,y:x1,y1;x2,y2;...{\n}
t_map *mx_level_parser(const char *map_file, const char *meta_file) {
    t_map *map = (t_map *)malloc(sizeof(t_map));

    map->array = mx_map_parser(map_file, &(map->w), &(map->h));

    char *str = mx_file_to_str(meta_file);

    char **rules = mx_strsplit(str, '\n', &(map->triggers_count));

    map->triggers =
        (t_trigger **)malloc(sizeof(t_trigger *) * map->triggers_count);

    int tmp;
    for (int i = 0; i < map->triggers_count; i++) {
        char **operands = mx_strsplit(rules[i], ':', &tmp);
        map->triggers[i] = (t_trigger *)malloc(sizeof(t_trigger));
        map->triggers[i]->type = *operands[0];
        map->triggers[i]->point = mx_parse_point(operands[1]);
        char **children =
            mx_strsplit(operands[2], ';', &(map->triggers[i]->child_count));
        map->triggers[i]->children = (t_point **)malloc(
            sizeof(t_point *) * map->triggers[i]->child_count);
        for (int j = 0; j < map->triggers[i]->child_count; j++) {
            map->triggers[i]->children[j] = mx_parse_point(children[j]);
        }
    }
    return map;
}
