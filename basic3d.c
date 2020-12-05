#include "basic3d.h"

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

void scalePoly(Poly *poly, float mx, float my, float mz)
{
    for (Ver3f *p = poly->vertices; p < poly->vertices + poly->vertexCount; p++) {
        p->x *= mx;
        p->y *= my;
        p->z *= mz;
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

void drawPoly(SDL_Renderer *renderer, Poly *poly)
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
