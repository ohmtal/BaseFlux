//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT

//-----------------------------------------------------------------------------
// Console Types source
//-----------------------------------------------------------------------------
#pragma once
#include "math/mMathFn.h"
#include <SDL3/SDL.h>
#ifndef _DYNAMIC_CONSOLETYPES_H_
#include "console/dynamicTypes.h"
#endif

#ifndef _ENGINEPRIMITIVES_H_
#include "console/enginePrimitives.h"
#endif

#ifndef _ENGINESTRUCTS_H_
#include "console/engineStructs.h"
#endif

typedef SDL_FColor Color4F;
typedef SDL_Color Color;
typedef SDL_FRect RectF;
typedef SDL_Rect RectI;
typedef SDL_Point Point2I;
typedef SDL_FPoint Point2F;

// from OhmFlux
struct Point3F {
    F32 x = 0.f, y=0.f, z=0.f;

    F32 distSq(const Point3F& other) const {
        F32 dx = x - other.x;
        F32 dy = y - other.y;
        F32 dz = z - other.z;
        return dx*dx + dy*dy + dz*dz;
    }

    F32 lenSquared() const {
        return (x * x + y * y + z * z);
    }
    F32 len() const {
        return sqrt(x*x + y*y + z*z);
    }

    Point3F operator+(const Point3F& v) const { return {x + v.x, y + v.y, z + v.z}; }
    Point3F operator-(const Point3F& v) const { return {x - v.x, y - v.y, z - v.z}; }
    Point3F operator*(const Point3F& v) const { return {x * v.x, y * v.y, z * v.z}; }
    Point3F operator/(const Point3F& v) const {
        return {
            x / v.x != 0.f ? v.x : 1e-9f,
            y / v.y != 0.f ? v.y : 1e-9f,
            z / v.z != 0.f ? v.z : 1e-9f
        };
    }

    bool operator==(const Point3F& v) const { return (x == v.x && y == v.y && z == v.z); }

    Point3F& operator+=(const Point2F& v) { x += v.x; y += v.y; return *this; }

    F32 dot(const Point3F& v) const { return (x * v.x + y * v.y + z * v.z); }

    Point2F toPoint2F() const { return { x,y}; }

    bool isZero() const { return (x == 0.f && y == 0.f && z == 0.f); }

    void normalize() {
        F32 l = len();
        if (l > 0.0f) { x /= l; y /= l; z /= l;}
    }
};

//


DECLARE_STRUCT(Color);
DECLARE_STRUCT(Color4F);
DECLARE_STRUCT( RectI );
DECLARE_STRUCT( RectF );
DECLARE_STRUCT( Point2I );
DECLARE_STRUCT( Point2F );
DECLARE_STRUCT( Point3F );

DefineConsoleType( TypeColor, Color )
DefineConsoleType( TypeColorF, Color4F )
DefineConsoleType( TypeRectI, RectI )
DefineConsoleType( TypeRectF, RectF )
DefineConsoleType( TypePoint2I, Point2I )
DefineConsoleType( TypePoint2F, Point2F )
DefineConsoleType( TypePoint3F, Point3F )

