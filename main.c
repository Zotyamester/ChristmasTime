#include "common.h"
#include "basic3d.h"
#include "imgeffect.h"

void sdl_init(char const *title, SDL_Window **pwindow, SDL_Renderer **prenderer) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_Log("Couldn't start SDL: %s", SDL_GetError());
        exit(1);
    }
    SDL_Window *window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    if (window == NULL) {
        SDL_Log("Couldn't create the window: %s", SDL_GetError());
        exit(1);
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        SDL_Log("Couldn't create the renderer: %s", SDL_GetError());
        exit(1);
    }
    SDL_RenderClear(renderer);

    *pwindow = window;
    *prenderer = renderer;
}

Uint32 timer(Uint32 ms, void *args) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;
}

int main(int argc, char *argv[]) {
    SDL_Window *window;
    SDL_Renderer *renderer;
    sdl_init("Christmas Time", &window, &renderer);

    SDL_TimerID id = SDL_AddTimer(20, timer, NULL);

    // TESTING

    Poly poly[3];
    loadPoly(&poly[0], "tree.txt");
    loadPoly(&poly[1], "tree.txt");
    loadPoly(&poly[2], "tree.txt");

    scalePoly(&poly[0], 100.0f, 100.0f, 100.0f);
    rotatePoly(&poly[0], 0.0f, 3.14f/4.0f, 0.0f);
    transformPoly(&poly[0], -110.0f, 0.0f, -110.0f);
    projectPoly(&poly[0], 800.0f);

    scalePoly(&poly[1], 100.0f, 100.0f, 100.0f);
    rotatePoly(&poly[1], 0.0f, 3.14f/4.0f, 0.0f);
    transformPoly(&poly[1], 0.0f, 0.0f, 0.0f);
    projectPoly(&poly[1], 800.0f);

    scalePoly(&poly[2], 100.0f, 100.0f, 100.0f);
    rotatePoly(&poly[2], 0.0f, 3.14f/4.0f, 0.0f);
    transformPoly(&poly[2], 110.0f, 0.0f, 110.0f);
    projectPoly(&poly[2], 800.0f);

    drawPoly(renderer, &poly[2]);
    SDL_RenderPresent(renderer);
    blurScreen(window, renderer);

    drawPoly(renderer, &poly[1]);
    SDL_RenderPresent(renderer);
    blurScreen(window, renderer);

    drawPoly(renderer, &poly[0]);

    SDL_RenderPresent(renderer);
    //zOrder(2, poly);
    //drawPolys(renderer, 2, poly);

    bool quit = false;
    while (!quit) {
        SDL_Event event;
        SDL_WaitEvent(&event);

        switch (event.type) {
            case SDL_USEREVENT:
                break;
            case SDL_QUIT:
                quit = true;
                break;
        }
    }

    SDL_RemoveTimer(id);

    SDL_Quit();

    return 0;
}
