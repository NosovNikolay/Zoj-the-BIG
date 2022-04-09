#pragma once

#include <fcntl.h> 
#include <stdlib.h>
#include <unistd.h>

#include "map.h"
#include "string_methods.h"

char *mx_file_to_str(const char *filename);

char **mx_map_parser(const char *filename, int *w, int *h);

t_map *mx_level_parser(const char *map_file, const char *meta_file);

t_point *mx_parse_point(char *coordinates);
