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
typedef struct Point3F
{
    float x;
    float y;
    float z;
} Point3F;

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

