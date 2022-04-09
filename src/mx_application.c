#include "application.h"
#include "state_menu.h"

static void clean_context(t_context *ctx) {
    if (ctx->state_type == ST_MENU)
        ctx->menu->cleanup(ctx);
    else if (ctx->state_type == ST_GAME)
        ctx->game->cleanup(ctx);
    Mix_FreeMusic(ctx->bg_music);
    IMG_Quit();
    SDL_DestroyRenderer(ctx->renderer);
    SDL_DestroyWindow(ctx->window);
    SDL_Quit();
}

static void init_context(t_context *ctx) {
    ctx->is_running = true;
    ctx->state_type = ST_MENU;
    ctx->delta_time = 0;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS |
                 SDL_INIT_AUDIO) != 0) {
        mx_printerr(
            "[ERROR]: Can't initialize SDL. Install Windows and play normal "
            "games you moron\n");
        clean_context(ctx);
        exit(EXIT_SUCCESS);
    }
    IMG_Init(IMG_INIT_PNG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    ctx->window = SDL_CreateWindow("ZOJ!", SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED, MX_WIDTH, MX_HEIGHT,
                                   SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
    ctx->renderer = SDL_CreateRenderer(
        ctx->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_GetCurrentDisplayMode(0, &ctx->display);
    ctx->game = NULL;
    ctx->save = mx_get_user_save();
    ctx->max_available_level = ctx->save->max_available_lvl;  
    ctx->current_level = ctx->save->last_visited_lvl;
    ctx->menu = mx_create_state_menu(ctx);
    ctx->bg_music = Mix_LoadMUS("resource/sounds/Bg.mp3");
    Mix_PlayMusic(ctx->bg_music, -1);
    if (ctx->menu == NULL) ctx->is_running = false;
}

void mx_run_main_loop(void) {
    t_context ctx;
    Uint32 curr_tick = SDL_GetTicks();
    Uint32 last_tick;
    init_context(&ctx);

    while (ctx.is_running == true) {
        last_tick = curr_tick;

        switch (ctx.state_type) {
            case ST_MENU:
                while (SDL_PollEvent(&ctx.event)) {
                    if (ctx.event.type == SDL_QUIT) ctx.is_running = false;
                    ctx.menu->input(&ctx);
                }
                ctx.menu->update(&ctx);
                ctx.menu->render(&ctx);
                break;
            case ST_GAME:
                while (SDL_PollEvent(&ctx.event)) {
                    if (ctx.event.type == SDL_QUIT) ctx.is_running = false;
                    ctx.game->input(&ctx);
                }
                ctx.game->update(&ctx);
                ctx.game->render(&ctx);
                if (ctx.state_type != ST_GAME) ctx.game->cleanup(&ctx);
                break;
            default:
                ctx.is_running = false;
                break;
        }
        curr_tick = SDL_GetTicks();
        ctx.delta_time = curr_tick - last_tick;
    }

    clean_context(&ctx);
}
