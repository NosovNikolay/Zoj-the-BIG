#pragma once

#include <fcntl.h> 
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "string_methods.h"

#define MX_FILE_NOT_OPENED_ERR "[ERROR]: the last save file was not opened"

#define MX_SAVE_PATH "resource/ohklptrlekiasiiovleo"

typedef struct s_user_save
{
    int max_available_lvl;
    int last_visited_lvl;
} t_user_save;

void mx_update_user_save(t_user_save *save);

t_user_save *mx_get_user_save();
