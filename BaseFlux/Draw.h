//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// BaseFlux Draw
//-----------------------------------------------------------------------------
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
    void DrawLine(SDL_Renderer *renderer, SDL_FPoint p1,SDL_FPoint p2 , SDL_Color color);
    void DrawRect(SDL_Renderer *renderer, SDL_FRect rect , SDL_Color color, bool fill);

    void DrawDebugText( SDL_Renderer *renderer,
                        float x, float y, const char* text,
                        float scale = 1.f, SDL_Color color = {100,100,100,255},
                        bool doShadow = false, SDL_Color shadowColor = { 60,60,60,255});
    void DrawCircle(SDL_Renderer *renderer, float radius, SDL_FPoint pos, SDL_Color color, bool fill);
    void DrawArc(SDL_Renderer *renderer, float radius, float startRad, float endRad, SDL_FPoint pos, SDL_Color color, bool fill);
    void DrawDonut(SDL_Renderer *renderer, float innerRadius, float outerRadius, SDL_FPoint pos, SDL_Color color, bool fill);
}; //namespace
