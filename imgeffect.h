#ifndef IMGEFFECT_H
#define IMGEFFECT_H

#include "common.h"

typedef unsigned char Matrix[HEIGHT][WIDTH];

void screenToMatrix(SDL_Surface *bmp, Matrix *mat);
void matrixToScreen(SDL_Renderer *renderer, Matrix *mat);
void blurScreen(SDL_Surface *bmp, SDL_Renderer *renderer);

#endif // IMGEFFECT_H
