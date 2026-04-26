#pragma once
#include <SDL3/SDL.h>
#include <sys/types.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#ifndef M_2PI // 2 * pi
#define M_2PI (2.0f * M_PI)
#endif

namespace BaseFlux {

    inline void DrawCircle(SDL_Renderer *renderer, float radius, SDL_FPoint pos, SDL_Color color, bool fill) {
        if (!renderer || radius <= 0) return;

        int segments = (int)(M_2PI * sqrtf(radius) * 2.0f);
        if (segments < 12) segments = 12;
        if (segments > 512) segments = 512;

        SDL_FColor colorF = { color.r/255.f, color.g/255.f, color.b/255.f, color.a/255.f };

        if (fill) {
            SDL_Vertex* vertices = (SDL_Vertex*)alloca(sizeof(SDL_Vertex) * (segments + 2));
            int* indices = (int*)alloca(sizeof(int) * (segments * 3));

            vertices[0] = { pos, colorF, {0,0} };
            for (int i = 0; i <= segments; i++) {
                float ang = i * (M_2PI / segments);
                vertices[i+1] = { {pos.x + cosf(ang) * radius, pos.y + sinf(ang) * radius}, colorF, {0,0} };
            }

            for (int i = 0; i < segments; i++) {
                indices[i * 3 + 0] = 0;
                indices[i * 3 + 1] = i + 1;
                indices[i * 3 + 2] = i + 2;
            }
            SDL_RenderGeometry(renderer, nullptr, vertices, segments + 2, indices, segments * 3);
        } else {
            SDL_FPoint* points = (SDL_FPoint*)alloca(sizeof(SDL_FPoint) * (segments + 1));
            for (int i = 0; i <= segments; i++) {
                float ang = i * (M_2PI / segments);
                points[i] = { pos.x + cosf(ang) * radius, pos.y + sinf(ang) * radius };
            }
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderLines(renderer, points, segments + 1);
        }
    }

    inline void DrawArc(SDL_Renderer *renderer, float radius, float startRad, float endRad, SDL_FPoint pos, SDL_Color color, bool fill) {
        if (!renderer || radius <= 0) return;

        float delta = endRad - startRad;
        int segments = (int)(fabsf(delta) * radius * 0.5f);
        if (segments < 4) segments = 4;     // Mindestmaß für Kurven
        if (segments > 512) segments = 512;

        SDL_FColor colorF = { color.r/255.f, color.g/255.f, color.b/255.f, color.a/255.f };

        if (fill) {
            SDL_Vertex* vertices = (SDL_Vertex*)alloca(sizeof(SDL_Vertex) * (segments + 2));
            int* indices = (int*)alloca(sizeof(int) * (segments * 3));

            vertices[0] = { pos, colorF, {0,0} };

            for (int i = 0; i <= segments; i++) {
                float ang = startRad + (i * (delta / segments));
                vertices[i+1] = { {pos.x + cosf(ang) * radius, pos.y + sinf(ang) * radius}, colorF, {0,0} };
            }

            for (int i = 0; i < segments; i++) {
                indices[i * 3 + 0] = 0;
                indices[i * 3 + 1] = i + 1;
                indices[i * 3 + 2] = i + 2;
            }
            SDL_RenderGeometry(renderer, nullptr, vertices, segments + 2, indices, segments * 3);
        } else {
            SDL_FPoint* points = (SDL_FPoint*)alloca(sizeof(SDL_FPoint) * (segments + 1));
            for (int i = 0; i <= segments; i++) {
                float ang = startRad + (i * (delta / segments));
                points[i] = { pos.x + cosf(ang) * radius, pos.y + sinf(ang) * radius };
            }
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderLines(renderer, points, segments + 1);
        }
    }


    inline void DrawDonut(SDL_Renderer *renderer, float innerRadius, float outerRadius, SDL_FPoint pos, SDL_Color color, bool fill) {
        if (!renderer || outerRadius <= 0) return;

        int segments = (int)(M_2PI * sqrtf(outerRadius) * 2.0f);
        if (segments < 12) segments = 12;
        if (segments > 512) segments = 512;

        SDL_FColor colorF = { color.r/255.f, color.g/255.f, color.b/255.f, color.a/255.f };

        if (fill) {
            int numVertices = (segments + 1) * 2;
            SDL_Vertex* vertices = (SDL_Vertex*)alloca(sizeof(SDL_Vertex) * numVertices);
            int* indices = (int*)alloca(sizeof(int) * segments * 6); // 2 Dreiecke pro Segment-Schritt

            for (int i = 0; i <= segments; i++) {
                float ang = i * (M_2PI / segments);
                float c = cosf(ang);
                float s = sinf(ang);
                vertices[i * 2 + 0] = { {pos.x + c * outerRadius, pos.y + s * outerRadius}, colorF, {0,0} };
                vertices[i * 2 + 1] = { {pos.x + c * innerRadius, pos.y + s * innerRadius}, colorF, {0,0} };
            }
            for (int i = 0; i < segments; i++) {
                int v0 = i * 2;
                int v1 = i * 2 + 1;
                int v2 = (i + 1) * 2;
                int v3 = (i + 1) * 2 + 1;

                indices[i * 6 + 0] = v0;
                indices[i * 6 + 1] = v1;
                indices[i * 6 + 2] = v2;

                indices[i * 6 + 3] = v1;
                indices[i * 6 + 4] = v3;
                indices[i * 6 + 5] = v2;
            }
            SDL_RenderGeometry(renderer, nullptr, vertices, numVertices, indices, segments * 6);
        } else {
            DrawCircle(renderer, outerRadius, pos, color, false);
            DrawCircle(renderer, innerRadius, pos, color, false);
        }
    }

}; //namespace
