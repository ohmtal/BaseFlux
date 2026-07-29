
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

    SDL_FColor SDL_ColorToFColor(SDL_Color color) {
        SDL_FColor fcolor;
        fcolor.r = color.r / 255.0f;
        fcolor.g = color.g / 255.0f;
        fcolor.b = color.b / 255.0f;
        fcolor.a = color.a / 255.0f;
        return fcolor;
    }

    void DrawLine(SDL_Renderer *renderer, SDL_FPoint p1,SDL_FPoint p2 , SDL_Color color) {
        SDL_Color oldColor;
        SDL_SetRenderDrawColor(renderer, color.r,color.g,color.b,color.a);
        SDL_RenderLine(renderer, p1.x, p1.y, p2.x , p2.y);
        SDL_GetRenderDrawColor(renderer,&oldColor.r, &oldColor.g, & oldColor.b, &oldColor.a);
    }


    void DrawLineThick(SDL_Renderer *renderer, float x1, float y1, float x2, float y2, float thickness, SDL_Color color) {
        float dx = x2 - x1;
        float dy = y2 - y1;
        float length = sqrtf(dx * dx + dy * dy);

        if (length == 0.0f) return;

        float nx = -dy / length * (thickness * 0.5f);
        float ny =  dx / length * (thickness * 0.5f);

        SDL_FColor colorF = SDL_ColorToFColor(color);

        SDL_Vertex vertices[4] = {
            { { x1 + nx, y1 + ny }, colorF, { 0 } },
            { { x1 - nx, y1 - ny }, colorF, { 0 } },
            { { x2 + nx, y2 + ny }, colorF, { 0 } },
            { { x2 - nx, y2 - ny }, colorF, { 0 } }
        };

        int indices[6] = { 0, 1, 2, 1, 2, 3 };

        SDL_RenderGeometry(renderer, NULL, vertices, 4, indices, 6);
    }



    void DrawRect(SDL_Renderer *renderer, SDL_FRect rect , SDL_Color color, bool fill) {
        SDL_Color oldColor;
        SDL_SetRenderDrawColor(renderer, color.r,color.g,color.b,color.a);
        if (fill) SDL_RenderFillRect(renderer, &rect);
        else SDL_RenderRect(renderer, &rect);
        SDL_GetRenderDrawColor(renderer,&oldColor.r, &oldColor.g, & oldColor.b, &oldColor.a);
    }


    void DrawDebugText( SDL_Renderer *renderer,
                        float x, float y, const char* text,
                       float scale /*= 1.f*/, SDL_Color color /*= {100,100,100,255}*/,
                        bool doShadow /*= false*/, SDL_Color shadowColor /*= { 60,60,60,255}*/)
    {
        SDL_Color oldColor;
        float oldScaleX, oldScaleY;
        SDL_GetRenderDrawColor(renderer,&oldColor.r, &oldColor.g, & oldColor.b, &oldColor.a);
        SDL_GetRenderScale(renderer, &oldScaleX, &oldScaleY);
        SDL_SetRenderScale(renderer, scale, scale);

        if (doShadow) {
            SDL_SetRenderDrawColor(renderer, shadowColor.r,shadowColor.g,shadowColor.b,shadowColor.a);
            SDL_RenderDebugText(renderer, (x + 1)/ scale, (y + 1) / scale, text);
        }
        SDL_SetRenderDrawColor(renderer, color.r,color.g,color.b,color.a);
        SDL_RenderDebugText(renderer, x / scale, y / scale, text);

        // restore old scale color
        SDL_SetRenderScale(renderer, oldScaleX, oldScaleY);
        SDL_SetRenderDrawColor(renderer, oldColor.r, oldColor.g, oldColor.b, oldColor.a);

    }

    void DrawCircle(SDL_Renderer *renderer, float radius, SDL_FPoint pos, SDL_Color color, bool fill) {
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

    void DrawArc(SDL_Renderer *renderer, float radius, float startRad, float endRad, SDL_FPoint pos, SDL_Color color, bool fill) {
        if (!renderer || radius <= 0) return;

        float delta = endRad - startRad;
        int segments = (int)(fabsf(delta) * radius * 0.5f);
        if (segments < 4) segments = 4;
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


    void DrawDonut(SDL_Renderer *renderer, float innerRadius, float outerRadius, SDL_FPoint pos, SDL_Color color, bool fill) {
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

    // ------------------ Lights helper --------------------
    SDL_Texture* CreatePointLightTexture(SDL_Renderer* renderer, int radius, SDL_Color color, bool diffuse) {
        int size = radius * 2;
        SDL_Surface* surface = SDL_CreateSurface(size, size, SDL_PIXELFORMAT_RGBA32);
        if (!surface) return nullptr;

        SDL_LockSurface(surface);
        Uint32* pixels = (Uint32*)surface->pixels;
        int pitchPixels = surface->pitch / 4;

        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                float dx = x - radius;
                float dy = y - radius;
                float distance = SDL_sqrtf(dx * dx + dy * dy);

                Uint32 pixelColor = 0x00000000;

                if (distance < radius) {
                    float intensity = 0.0f;

                    if (diffuse) {
                        // Normalize distance between 0.0 (center) and 1.0 (edge)
                        float distanceRatio = distance / radius;

                        // Cosine creates a smooth S-curve (soft flat peak at center, gentle fade to edge)
                        intensity = SDL_cosf(distanceRatio * (3.14159265f / 2.0f));
                    } else {
                        // linear falloff (1.0 at center, 0.0 at edge)
                        intensity = 1.0f - (distance / radius);
                    }

                    Uint8 r = (Uint8)(color.r * intensity);
                    Uint8 g = (Uint8)(color.g * intensity);
                    Uint8 b = (Uint8)(color.b * intensity);
                    Uint8 a = (Uint8)(color.a * intensity);

                    pixelColor = (r << 24) | (g << 16) | (b << 8) | a;
                }

                pixels[y * pitchPixels + x] = pixelColor;
            }
        }

        SDL_UnlockSurface(surface);

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);

        return texture;
    }

    SDL_Texture* CreateSpotlightTexture(SDL_Renderer* renderer, int radius, float coneAngleDegrees, SDL_Color color, bool diffuse ) {
        int size = radius * 2;
        SDL_Surface* surface = SDL_CreateSurface(size, size, SDL_PIXELFORMAT_RGBA32);
        if (!surface) return nullptr;

        SDL_LockSurface(surface);
        Uint32* pixels = (Uint32*)surface->pixels;
        int pitchPixels = surface->pitch / 4;

        float halfConeRad = (coneAngleDegrees * (3.14159265f / 180.0f)) / 2.0f;

        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                float dx = x - radius;
                float dy = y - radius;
                float distance = SDL_sqrtf(dx * dx + dy * dy);

                Uint32 pixelColor = 0x00000000;

                if (distance < radius && distance > 0) {
                    float pixelAngle = SDL_atan2f(dx, dy);
                    float angleDiff = SDL_fabsf(pixelAngle);
                    float finalIntensity = 0.f;
                    if (angleDiff <= halfConeRad) {
                        if (diffuse) {
                            // Smooth Cosine Diffusion:
                            // This normalizes the angle between 0.0 (center) and 1.0 (edge)
                            float angleRatio = angleDiff / halfConeRad;

                            // Cosine creates a smooth "S-curve" profile (soft peak, soft edge)
                            float angularIntensity = SDL_cosf(angleRatio * (3.14159265f / 2.0f));

                            // --- Combine intensities (you can also smooth out the distance falloff here) ---
                            float distanceRatio = distance / radius;
                            float distanceIntensity = SDL_cosf(distanceRatio * (3.14159265f / 2.0f));

                            // Final combined soft intensity
                            finalIntensity = distanceIntensity * angularIntensity;
                        } else {
                            // make a hard ray ...
                            // Distance falloff (center to edge)
                            float distanceIntensity = 1.0f - (distance / radius);

                            // Angular falloff (cone center to cone edge smoothing)
                            float angularIntensity = 1.0f - (angleDiff / halfConeRad);

                            // Combine intensities
                            finalIntensity = distanceIntensity * angularIntensity;
                        }

                        Uint8 r = (Uint8)(color.r * finalIntensity);
                        Uint8 g = (Uint8)(color.g * finalIntensity);
                        Uint8 b = (Uint8)(color.b * finalIntensity);
                        Uint8 a = (Uint8)(color.a * finalIntensity);

                        pixelColor = (r << 24) | (g << 16) | (b << 8) | a;
                    }
                }
                pixels[y * pitchPixels + x] = pixelColor;
            }
        }

        SDL_UnlockSurface(surface);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);

        return texture;
    }


}; //namespace
