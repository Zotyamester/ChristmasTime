#ifndef IMGEFFECT_H
#define IMGEFFECT_H

#include "common.h"

typedef unsigned char Matrix[HEIGHT][WIDTH];

void screenToMatrix(SDL_Window *window, Matrix *mat);
void matrixToScreen(SDL_Renderer *renderer, Matrix *mat);
void blurScreen(SDL_Window *window, SDL_Renderer *renderer);

#endif // IMGEFFECT_H
