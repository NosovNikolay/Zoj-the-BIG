#include "user_save.h"

t_user_save *mx_get_user_save() {
    t_user_save *save = (t_user_save *)malloc(sizeof(t_user_save));
    FILE *f = fopen(MX_SAVE_PATH, "rb");
    if(!f) {
        save->last_visited_lvl = 1;
        save->max_available_lvl = 1;
        return save;
    }
    fread(save, sizeof(t_user_save), 1, f);
    fclose(f);
    return save;
}

void mx_update_user_save(t_user_save *save) {
    FILE *f = fopen(MX_SAVE_PATH, "wb");
    if(!f) {
        write(2, MX_FILE_NOT_OPENED_ERR, strlen(MX_FILE_NOT_OPENED_ERR));
        return;
    }
    fwrite(save, sizeof(t_user_save), 1, f);
    fclose(f);
}
