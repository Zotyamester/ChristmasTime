#include "basic3d.h"

void loadPoly(Poly *poly, const char *filename)
{
    FILE *fp = fopen(filename, "r");
    poly->dx = poly->dy = poly->dz = 0;
    fscanf(fp, "%d %d", &poly->vertexCount, &poly->edgeCount);
    poly->vertices = malloc(poly->vertexCount * sizeof(Ver3f));
    for (int i = 0; i < poly->vertexCount; i++) {
        fscanf(fp, "%f %f %f",
               &poly->vertices[i].x,
               &poly->vertices[i].y,
               &poly->vertices[i].z);
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
    for (Ver3f *v = poly->vertices; v < poly->vertices + poly->vertexCount; v++) {
        v->x += dx;
        v->y += dy;
        v->z += dz;
    }
    poly->dx += dx;
    poly->dy += dy;
    poly->dz += dz;
}

void scalePoly(Poly *poly, float mx, float my, float mz)
{
    for (Ver3f *v = poly->vertices; v < poly->vertices + poly->vertexCount; v++) {
        v->x = (v->x - poly->dx) * mx + poly->dx;
        v->y = (v->y - poly->dy) * my + poly->dy;
        v->z = (v->z - poly->dz) * mz + poly->dz;
    }
}

void rotatePoly(Poly *poly, float xa, float ya, float za)
{
    for (Ver3f *v = poly->vertices; v < poly->vertices + poly->vertexCount; v++) {
        float ox = v->x - poly->dx, oy = v->y - poly->dy, oz = v->z - poly->dz;
        Vec3 rx = {ox, oz * sin(xa) + oy * cos(xa), oz * cos(xa) - oy * sin(xa)};
        Vec3 ry = {rx.x * cos(ya) - rx.z * sin(ya), rx.y, rx.x * sin(ya) + rx.z * cos(ya)};
        Vec3 rz = {ry.x * cos(za) - ry.y * sin(za), ry.x * sin(za) + ry.y * cos(za), ry.z};
        v->x = rz.x + poly->dx;
        v->y = rz.y + poly->dy;
        v->z = rz.z + poly->dz;
    }
}

void projectPoly(Poly *poly)
{
    for (Ver3f *v = poly->vertices; v < poly->vertices + poly->vertexCount; v++) {
        v->px = +VIEWPORT * v->x / (v->z + VIEWPORT) + WIDTH / 2;
        v->py = -VIEWPORT * v->y / (v->z + VIEWPORT) + HEIGHT / 2;
    }
}

void drawLine(SDL_Renderer *renderer, Ver3f *i, Ver3f *j)
{
    lineRGBA(renderer, i->px, i->py, j->px, j->py, 255, 255, 255, 255);
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
    for (Poly *poly = polys; poly < polys + polyCount; poly++) {
        drawPoly(renderer, poly);
    }
}
