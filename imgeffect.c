#include "imgeffect.h"

void screenToMatrix(SDL_Surface *bmp, Matrix *mat)
{
    for (int y = 0; y < HEIGHT; y++) {
        Uint32 *row = (Uint32 *) ((char*) bmp->pixels + y*bmp->pitch);
        for (int x = 0; x < WIDTH; x++) {
            mat[0][y][x] = (row[x] >> bmp->format->Rshift) & 0xFF << bmp->format->Rloss;
            mat[1][y][x] = (row[x] >> bmp->format->Gshift) & 0xFF << bmp->format->Gloss;
            mat[2][y][x] = (row[x] >> bmp->format->Bshift) & 0xFF << bmp->format->Bloss;
        }
    }
}

void matrixToScreen(SDL_Renderer *renderer, Matrix *mat)
{
    SDL_Surface *bmp = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0, 0, 0, 0);
    for (int y = 0; y < HEIGHT; y++) {
        Uint32 *row = (Uint32 *) ((char *) bmp->pixels + y * bmp->pitch);
        for (int x = 0; x < WIDTH; x++) {
            Uint32 num;

            num = (mat[0][y][x] >> bmp->format->Rloss << bmp->format->Rshift)
                 | (mat[1][y][x] >> bmp->format->Gloss << bmp->format->Gshift)
                 | (mat[2][y][x] >> bmp->format->Bloss << bmp->format->Bshift);
            row[x] = num;
        }
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, bmp);
    SDL_FreeSurface(bmp);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_DestroyTexture(texture);
}

void blurScreen(SDL_Surface *bmp, SDL_Renderer *renderer)
{
    Matrix *src = malloc(3 * sizeof(Matrix));
    Matrix *dst = malloc(3 * sizeof(Matrix));
    screenToMatrix(bmp, src);
    screenToMatrix(bmp, dst);
    for (int c = 0; c < 3; c++) {
        for (int y = 1; y < HEIGHT - 1; y++) {
            for (int x = 1; x < WIDTH - 1; x++) {
                dst[c][y][x] = 1.0 / 9.0 * (
                    src[c][y - 1][x - 1] + src[c][y - 1][x] + src[c][y - 1][x + 1] +
                    src[c][y][x - 1] + src[c][y][x] + src[c][y][x + 1] +
                    src[c][y + 1][x - 1] + src[c][y + 1][x] + src[c][y + 1][x + 1]
                );
            }
        }
    }
    matrixToScreen(renderer, dst);
    free(src);
    free(dst);
}
