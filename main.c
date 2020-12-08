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
    SDL_Surface *buffer = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0, 0, 0, 0);
    SDL_Renderer *bufRenderer = SDL_CreateSoftwareRenderer(buffer);

    SDL_TimerID id = SDL_AddTimer(16, timer, NULL);

    // TESTING

    Poly poly[6];
    loadPoly(&poly[0], "tree.txt");
    loadPoly(&poly[1], "tree.txt");
    loadPoly(&poly[2], "tree.txt");
    loadPoly(&poly[3], "tree.txt");
    loadPoly(&poly[4], "tree.txt");
    loadPoly(&poly[5], "tree.txt");

    scalePoly(&poly[0], 100.0f, 100.0f, 100.0f);
    rotatePoly(&poly[0], 0.0f, 3.14f/4.0f, 0.0f);
    transformPoly(&poly[0], -200.0f, 0.0f, -300.0f);
    projectPoly(&poly[0]);
    scalePoly(&poly[1], 100.0f, 100.0f, 100.0f);
    rotatePoly(&poly[1], 0.0f, 3.14f/4.0f, 0.0f);
    transformPoly(&poly[1], 200.0f, 0.0f, -300.0f);
    projectPoly(&poly[1]);

    scalePoly(&poly[2], 100.0f, 100.0f, 100.0f);
    rotatePoly(&poly[2], 0.0f, 3.14f/4.0f, 0.0f);
    transformPoly(&poly[2], -200.0f, 0.0f, 300.0f);
    projectPoly(&poly[2]);
    scalePoly(&poly[3], 100.0f, 100.0f, 100.0f);
    rotatePoly(&poly[3], 0.0f, 3.14f/4.0f, 0.0f);
    transformPoly(&poly[3], 200.0f, 0.0f, 300.0f);
    projectPoly(&poly[3]);

    scalePoly(&poly[4], 100.0f, 100.0f, 100.0f);
    rotatePoly(&poly[4], 0.0f, 3.14f/4.0f, 0.0f);
    transformPoly(&poly[4], -200.0f, 0.0f, 900.0f);
    projectPoly(&poly[4]);
    scalePoly(&poly[5], 100.0f, 100.0f, 100.0f);
    rotatePoly(&poly[5], 0.0f, 3.14f/4.0f, 0.0f);
    transformPoly(&poly[5], 200.0f, 0.0f, 900.0f);
    projectPoly(&poly[5]);

    bool quit = false;
    while (!quit) {
        SDL_Event event;
        SDL_WaitEvent(&event);

        switch (event.type) {
            case SDL_USEREVENT:
                SDL_SetRenderDrawColor(bufRenderer, 0, 0, 0, 255);
                SDL_RenderClear(bufRenderer);

                for (int i = 0; i < 6; i++) {
                    rotatePoly(&poly[i], 0.0f, 3.14f/100.0f, 0.0f);
                    projectPoly(&poly[i]);
                }

                drawPoly(bufRenderer, &poly[5]);
                drawPoly(bufRenderer, &poly[4]);
                /*SDL_RenderPresent(bufRenderer);
                blurScreen(buffer, bufRenderer);*/

                drawPoly(bufRenderer, &poly[3]);
                drawPoly(bufRenderer, &poly[2]);
                /*SDL_RenderPresent(bufRenderer);
                blurScreen(buffer, bufRenderer);*/

                drawPoly(bufRenderer, &poly[1]);
                drawPoly(bufRenderer, &poly[0]);

                SDL_RenderPresent(bufRenderer);
                SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, buffer);
                SDL_RenderCopy(renderer, texture, NULL, NULL);
                SDL_DestroyTexture(texture);
                SDL_RenderPresent(renderer);
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
