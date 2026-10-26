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
    void DrawLine(SDL_Renderer *renderer, const SDL_FPoint& p1,const SDL_FPoint& p2 , const SDL_Color& color);
    void DrawLineThick(SDL_Renderer *renderer, float x1, float y1, float x2, float y2, float thickness, const SDL_Color& color);
    void DrawLineThick(SDL_Renderer *renderer, const SDL_FPoint& p1, const SDL_FPoint& p2 , float thickness, const SDL_Color& color) {
        DrawLineThick(renderer, p1.x, p1.y, p2.x, p2.y, thickness, color);
    }
    void DrawRect(SDL_Renderer *renderer, const SDL_FRect& rect , const SDL_Color& color, bool fill);

    void DrawDebugText( SDL_Renderer *renderer,
                        float x, float y, const char* text,
                        float scale = 1.f, const SDL_Color& color = {100,100,100,255},
                        bool doShadow = false, const SDL_Color& shadowColor = { 60,60,60,255});
    void DrawCircle(SDL_Renderer *renderer, float radius, const SDL_FPoint& pos, const SDL_Color& color, bool fill);
    void DrawArc(SDL_Renderer *renderer, float radius, float startRad, float endRad, const SDL_FPoint& pos, const SDL_Color& color, bool fill);
    void DrawDonut(SDL_Renderer *renderer, float innerRadius, float outerRadius, const SDL_FPoint& pos, const SDL_Color& color, bool fill);

    // Lights
    SDL_Texture* CreatePointLightTexture(SDL_Renderer* renderer, int radius, const SDL_Color& color, bool diffuse = true);
    SDL_Texture* CreateRayLightTexture(SDL_Renderer* renderer, int width,int height, const SDL_Color& color, bool diffuse = true);
    SDL_Texture* CreateSpotlightTexture(SDL_Renderer* renderer, int radius, float coneAngleDegrees, const SDL_Color& color
                , bool diffuse = true, bool damping = false);


}; //namespace
