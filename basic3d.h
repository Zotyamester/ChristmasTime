#ifndef BASIC3D_H
#define BASIC3D_H

#include "common.h"

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

void loadPoly(Poly *poly, const char *filename);
void unloadPoly(Poly *poly);
void transformPoly(Poly *poly, float dx, float dy, float dz);
void scalePoly(Poly *poly, float mx, float my, float mz);
void rotatePoly(Poly *poly, float xa, float ya, float za);
void projectPoly(Poly *poly, float viewport);
int comparePolys(const void *a, const void *b);
void zOrder(int polyCount, Poly *polys);
void drawLine(SDL_Renderer *renderer, Ver3f *i, Ver3f *j);
void drawPoly(SDL_Renderer *renderer, Poly *poly);
void drawPolys(SDL_Renderer *renderer, int polyCount, Poly *polys);

#endif // BASIC3D_H
