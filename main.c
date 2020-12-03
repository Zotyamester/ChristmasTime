#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

enum { WIDTH = 800, HEIGHT = 800 };

const int FASZOM = 10;

typedef struct Vec3 {
    float x, y, z;
} Vec3;

typedef struct Ver3f {
    float x, y, z;
    int px, py;
} Ver3f;

typedef struct Edge {
    int i, j;
} Edge;

typedef struct Poly {
    int vertexCount;
    Ver3f *vertices;
    float minZ, maxZ;
    int edgeCount;
    Edge *edges;
} Poly;

void loadPoly(Poly *poly, const char *filename)
{
    FILE *fp = fopen(filename, "r");
    fscanf(fp, "%d %d", &poly->vertexCount, &poly->edgeCount);
    poly->vertices = malloc(poly->vertexCount * sizeof(Ver3f));
    poly->minZ = ~0;
    poly->maxZ = 0;
    for (int i = 0; i < poly->vertexCount; i++) {
        fscanf(fp, "%f %f %f",
               &poly->vertices[i].x,
               &poly->vertices[i].y,
               &poly->vertices[i].z);
        if (poly->vertices[i].z < poly->minZ) {
            poly->minZ = poly->vertices[i].z;
        } else if (poly->vertices[i].z > poly->maxZ) {
            poly->maxZ = poly->vertices[i].z;
        }
    }
    poly->edges = malloc(poly->edgeCount * sizeof(Edge));
    for (int i = 0; i < poly->edgeCount; i++) {
        fscanf(fp, "%d %d",
               &poly->edges[i].i,
               &poly->edges[i].j);
    }
    fclose(fp);
}

void unloadPoly(Poly *poly)
{
    free(poly->vertices);
    free(poly->edges);
}

void transformPoly(Poly *poly, float dx, float dy, float dz)
{
    for (Ver3f *p = poly->vertices; p < poly->vertices + poly->vertexCount; p++) {
        p->x += dx;
        p->y += dy;
        p->z += dz;
    }
}

void rotatePoly(Poly *poly, float xa, float ya, float za)
{
    for (Ver3f *p = poly->vertices; p < poly->vertices + poly->vertexCount; p++) {
        Vec3 rx = {p->x, p->z * sin(xa) + p->y * cos(xa), p->z * cos(xa) - p->y * sin(xa)};
        Vec3 ry = {rx.x * cos(ya) - rx.z * sin(ya), rx.y, rx.x * sin(ya) + rx.z * cos(ya)};
        Vec3 rz = {ry.x * cos(za) - ry.y * sin(za), ry.x * sin(za) + ry.y * cos(za), ry.z};
        p->x = rz.x;
        p->y = rz.y;
        p->z = rz.z;
    }
}

void projectPoly(Poly *poly, float viewport)
{
    for (Ver3f *p = poly->vertices; p < poly->vertices + poly->vertexCount; p++) {
        p->px = +viewport * p->x / (p->z + viewport) + WIDTH / 2;
        p->py = -viewport * p->y / (p->z + viewport) + HEIGHT / 2;
    }
}

int comparePolys(const void *a, const void *b)
{
    float aMinZ = ((Poly *)a)->minZ, aMaxZ = ((Poly *)a)->maxZ;
    float bMinZ = ((Poly *)b)->minZ, bMaxZ = ((Poly *)b)->maxZ;
    if (aMinZ < bMinZ)
        return -1;
    if (aMinZ > bMinZ)
        return 1;
    if (aMaxZ < bMaxZ)
        return -1;
    if (aMaxZ > bMaxZ)
        return 1;
    return 0;
}

void zOrder(int polyCount, Poly *polys)
{
    qsort(polys, polyCount, sizeof(Poly), comparePolys);
}

void drawLine(SDL_Renderer *renderer, Ver3f *i, Ver3f *j)
{
    aalineRGBA(renderer, i->px, i->py, j->px, j->py, 255, 255, 255, 255);
}

void drawPoly(SDL_Renderer *renderer,Poly *poly)
{
    for (Edge *e = poly->edges; e < poly->edges + poly->edgeCount; e++) {
        Ver3f *i = &poly->vertices[e->i], *j = &poly->vertices[e->j];
        drawLine(renderer, i, j);
    }
}

void drawPolys(SDL_Renderer *renderer, int polyCount, Poly *polys)
{
    for (Poly *p = polys; p < polys + polyCount; p++) {
        drawPoly(renderer, p);
    }
}

typedef unsigned char Matrix[HEIGHT][WIDTH];

void screenToMatrix(SDL_Window *window, Matrix *mat)
{
    SDL_Surface *bmp = SDL_GetWindowSurface(window);
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

void blurScreen(SDL_Window *window, SDL_Renderer *renderer)
{
    Matrix *src = malloc(3 * sizeof(Matrix));
    Matrix *dst = malloc(3 * sizeof(Matrix));
    screenToMatrix(window, src);
    screenToMatrix(window, dst);
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
    loadPoly(&poly[0], "cube.txt");
    loadPoly(&poly[1], "cube.txt");
    loadPoly(&poly[2], "cube.txt");

    rotatePoly(&poly[0], 0.0f, 3.14f/4.0f, 0.0f);
    transformPoly(&poly[0], -110.0f, 0.0f, -110.0f);
    projectPoly(&poly[0], 800.0f);

    rotatePoly(&poly[1], 0.0f, 3.14f/4.0f, 0.0f);
    transformPoly(&poly[1], 110.0f, 0.0f, 110.0f);
    projectPoly(&poly[1], 800.0f);

    rotatePoly(&poly[2], 0.0f, 3.14f/4.0f, 0.0f);
    transformPoly(&poly[2], 0.0f, 0.0f, 110.0f);
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
