//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// BaseFlux Basic Collision
//-----------------------------------------------------------------------------
#pragma once
#include <SDL3/SDL.h>
#include <cmath>

namespace BaseFlux::Collision {
    // -----------------------------------------------------------------------------
    struct Info {
        float mOverlap = 0.0f;             // how much we overlap
        SDL_FPoint mNormal = {0.0f, 0.0f};  // the direction where the collision come from
    };
    // -----------------------------------------------------------------------------
    bool getInfoRectF(const SDL_FRect& a, const SDL_FRect& b, Info& collisionInfo) {
        if (!SDL_HasRectIntersectionFloat(&a,&b)) {
            return false;
        }
        // overlap calculation
        float aHalfW = a.w / 2.0f;
        float bHalfW = b.w/ 2.0f;
        float diffX = (a.x + aHalfW) - (b.x + bHalfW);
        float overlapX = (aHalfW + bHalfW) - std::abs(diffX);

        float aHalfH = a.h/ 2.0f;
        float bHalfH = b.h / 2.0f;
        float diffY = (a.y + aHalfH) - (b.y + bHalfH);
        float overlapY = (aHalfH + bHalfH) - std::abs(diffY);


        // smallest check
        if (overlapX < overlapY) {
            collisionInfo.mOverlap = overlapX;
            collisionInfo.mNormal.x = (diffX > 0.0f) ? 1.0f : -1.0f; // 1 = von rechts, -1 = von links
            collisionInfo.mNormal.y = 0.0f;
        } else {
            collisionInfo.mOverlap = overlapY;
            collisionInfo.mNormal.x = 0.0f;
            collisionInfo.mNormal.y = (diffY > 0.0f) ? 1.0f : -1.0f; // 1 = von unten, -1 = von oben
        }
        return true;
    }
    // -----------------------------------------------------------------------------
    /*
     * set a at new position by collisionInfo
     */
    void solveOberlap( SDL_FRect& a,  const Info& collisionInfo) {
        a.x += collisionInfo.mNormal.x * collisionInfo.mOverlap;
        a.y += collisionInfo.mNormal.y * collisionInfo.mOverlap;
    }


    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    /**
     * get a line collision with an AABB rectangle.
     * @return true if collided and solved, false otherwise.
     * @param result Fills collision info
     */
    bool getInfoRectLine(const SDL_FRect& rect, float x1, float y1, float x2, float y2, Info& result) {
        float ix1 = x1, iy1 = y1, ix2 = x2, iy2 = y2;
        if (!SDL_GetRectAndLineIntersectionFloat(&rect, &ix1, &iy1, &ix2, &iy2)) {
            return false;
        }

        float lineDx = x2 - x1;
        float lineDy = y2 - y1;

        float normalX = -lineDy;
        float normalY = lineDx;

        float len = std::sqrt(normalX * normalX + normalY * normalY);
        if (len > 0.0f) {
            normalX /= len;
            normalY /= len;
        } else {
            return false;
        }

        float rectCenterX = rect.x + rect.w / 2.0f;
        float rectCenterY = rect.y + rect.h / 2.0f;
        float toObjectX = rectCenterX - x1;
        float toObjectY = rectCenterY - y1;

        float side = toObjectX * normalX + toObjectY * normalY;
        if (side < 0.0f) {
            normalX = -normalX;
            normalY = -normalY;
        }

        float radiusProj = (rect.w / 2.0f) * std::abs(normalX) + (rect.h / 2.0f) * std::abs(normalY);

        float distanceToLine = std::abs(toObjectX * normalX + toObjectY * normalY);

        result.mNormal = {normalX,normalY};
        result.mOverlap = radiusProj - distanceToLine + 0.1f;

        if (result.mOverlap > 0.0f) {
            return true;
        }

        return false;
    }


    /**
     * Solves a line collision with an AABB rectangle.
     * @return true if collided and solved, false otherwise.
     * @param result Fills the top-left corner (x, y) of the corrected position.
     */
    bool solveLineCollision(const SDL_FRect& rect, float x1, float y1, float x2, float y2, SDL_FPoint& result) {

        Info info;
        if (!getInfoRectLine(rect, x1, y1, x2, y2, info)) return false;

        result.x = rect.x + (info.mNormal.x * info.mOverlap);
        result.y = rect.y + (info.mNormal.y * info.mOverlap);
        return true;
    }


} //namespace
