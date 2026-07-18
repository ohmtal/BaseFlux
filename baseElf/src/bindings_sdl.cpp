//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// ElfScript BaseFlux/SDL3 binding
//-----------------------------------------------------------------------------
#include <SDL3/SDL.h>
#include "console/engineAPI.h"
#include "console/consoleExtras.h"
#include "core/volume.h"
#include "math/mMathFn.h"

#include <string>
#include <format>

#include "BaseFlux/Main.h"
#include "BaseFlux/Draw.h"
#include "BaseFlux/Collision.h"
#include <console/script.h>

#include "ConsoleTypes.h"
#include "ColorConstants.h"


extern BaseFlux::Main app;
extern SDL_Point gMousePos;


/* constant example:
 Con::registerEnumS32<MyEnum>("$MyEnum::", true);
 Con::setScriptConstant("_LEFT_", 1); //real constant using preprocessor*
 Con::setScriptConstant("_RIGHT_", 2); //real constant using preprocessor
*/



// -----------------------------------------------------------------------------
void registerColors() {
    const String prefix = "";

    const std::vector<std::pair<String, Color>> colors = {
        {"LIGHTGRAY", LIGHTGRAY},
        {"GRAY", GRAY},
        {"DARKGRAY", DARKGRAY},
        {"YELLOW", YELLOW},
        {"GOLD", GOLD},
        {"ORANGE", ORANGE},
        {"PINK", PINK},
        {"RED", RED},
        {"MAROON", MAROON},
        {"GREEN", GREEN},
        {"LIME", LIME},
        {"DARKGREEN", DARKGREEN},
        {"SKYBLUE", SKYBLUE},
        {"BLUE", BLUE},
        {"DARKBLUE", DARKBLUE},
        {"PURPLE", PURPLE},
        {"VIOLET", VIOLET},
        {"DARKPURPLE", DARKPURPLE},
        {"BEIGE", BEIGE},
        {"BROWN", BROWN},
        {"DARKBROWN", DARKBROWN},
        {"WHITE", WHITE},
        {"BLACK", BLACK},
        {"BLANK", BLANK},
        {"MAGENTA", MAGENTA},
        {"RAYWHITE", RAYWHITE},
        {"SLATEGRAY", SLATEGRAY},
        {"SEABLUE", SEABLUE}
    };


    for (std::size_t i = 0; i < colors.size(); ++i) {
        String fullName = prefix + colors[i].first;
        Color value = static_cast<Color>(colors[i].second);
        ConsoleBaseType* type = ConsoleBaseType::getType(TypeColor);

        if (type) {
            const char* colorStrP = type->getData(&value, nullptr, 0);
            std::string colorString = std::format("\"{}\"", colorStrP);
            Con::setScriptConstant(fullName.c_str(), colorString );
            // Con::printf("DEBUG key value: %s => %s", fullName.c_str(), colorString.c_str());
        }
    }

}


// -----------------------------------------------------------------------------
// =============================================================================
//  SettingsObject (singelton)
// =============================================================================
class SettingsObject: public SimObject
{
    typedef SimObject Parent;
    BaseFlux::Settings mSettings;

    StringTableEntry mCompany;
    StringTableEntry mCaption;
    StringTableEntry mVersion;
    StringTableEntry mIconFileName;
    StringTableEntry mAssetPath;
    StringTableEntry mSoundPathAppend;
    StringTableEntry mTexturePathAppend;
    StringTableEntry mIniFileName;
public:
    DECLARE_CONOBJECT(SettingsObject);
    SettingsObject() {
        mSettings = app.getSettings(); //create a copy
        mCompany = StringTable->insert(mSettings.Company.c_str());
        mCaption = StringTable->insert(mSettings.Caption.c_str());
        mVersion  = StringTable->insert(mSettings.Version.c_str());
        mIconFileName  = StringTable->insert(mSettings.IconFilename.c_str());
        mAssetPath  = StringTable->insert(mSettings.AssetPath.c_str());
        mSoundPathAppend  = StringTable->insert(mSettings.SoundPathAppend.c_str());
        mTexturePathAppend  = StringTable->insert(mSettings.TexturePathAppend.c_str());
        mIniFileName  = StringTable->insert(mSettings.IniFileName.c_str());
    }

    void Sync() {
        mSettings.Company = mCompany;
        mSettings.Caption = mCaption;
        mSettings.Version = mVersion;
        mSettings.IconFilename = mIconFileName;
        mSettings.AssetPath = mAssetPath;
        mSettings.SoundPathAppend = mSoundPathAppend;
        mSettings.TexturePathAppend = mTexturePathAppend;
        mSettings.IniFileName = mIniFileName;
        app.getSettings() = mSettings;
    }


    static void initPersistFields() {
          Parent::initPersistFields();
        addField("ScreenSize", TypePoint2I, Offset(mSettings.ScreenSize,SettingsObject));
        addField("FpsLimit", TypeS32, Offset(mSettings.FpsLimit,SettingsObject));
        addField("WindowMaximized", TypeBool, Offset(mSettings.WindowMaximized,SettingsObject));
        addField("FullScreen", TypeBool, Offset(mSettings.FullScreen,SettingsObject));
        addField("EnableVSync", TypeBool, Offset(mSettings.EnableVSync,SettingsObject));
        //FIXME ?!
        addField("Company", TypeString, Offset(mCompany,SettingsObject));
        addField("Caption", TypeString, Offset(mCaption,SettingsObject));
        addField("Version", TypeString, Offset(mVersion,SettingsObject));

        addField("IconFilename", TypeString, Offset( mIconFileName ,SettingsObject));

        addField("AssetPath", TypeString, Offset(mAssetPath,SettingsObject));
        addField("SoundPathAppend", TypeString, Offset(mSoundPathAppend,SettingsObject));
        addField("TexturePathAppend", TypeString, Offset(mTexturePathAppend,SettingsObject));

        addField("EnableDockSpace", TypeBool, Offset(mSettings.EnableDockSpace,SettingsObject));
        addField("IniFileName", TypeString, Offset(mIniFileName,SettingsObject));

        addField("sdlWindowFlagsOverwrite", TypeS32, Offset(mSettings.sdlWindowFlagsOverwrite,SettingsObject));

        addField("clearColor", TypeColor, Offset(mSettings.clearColor,SettingsObject));
    }
    // std::string getPrefsPath();
    // std::string getSafeCompany();
    // std::string getSafeCaption();
};
IMPLEMENT_CONOBJECT(SettingsObject);
DefineEngineMethod(SettingsObject, sync, void, (),,"Settings must be synced to get active!") {
    object->Sync();
}

SettingsObject* gSettingsObject;

// =============================================================================
//  TextureObject
// =============================================================================
// TODO: SDL_RenderTextureRotated
class TextureObject : public SimObject
{
    typedef SimObject Parent;
    StringTableEntry mFileName=nullptr;
    SDL_Texture* mTexture;
public:
    DECLARE_CONOBJECT(TextureObject);
    TextureObject() {
         mFileName = StringTable->EmptyString();
    }
    bool onAdd() override {
        mTexture = app.getTexture(mFileName);
        if (!mTexture) return false;
        return Parent::onAdd();
    }
    // void onRemove() override;

    SDL_Texture* get() { return mTexture; };
    static void initPersistFields() {
        Parent::initPersistFields();
        addField("fileName", TypeString, Offset(mFileName,TextureObject));
    }


    RectF getAtlasRect(S32 rowCount, S32 colCount, S32 index) {
        RectF result = {0.f,0.f,0.f,0.f};
        if (!mTexture || colCount < 1 || rowCount < 1 ) return result;
        F32 texW = (F32)mTexture->w;
        F32 texH = (F32)mTexture->h;

        result.w = texW / colCount;
        result.h = texH / rowCount;

        S32 cellX = index % colCount;
        S32 cellY = index / colCount;

        result.x = cellX * result.w;
        result.y = cellY * result.h;

        return result;
    }


//     bool SDL_RenderTextureRotated(SDL_Renderer *renderer, SDL_Texture *texture,
//                                   const SDL_FRect *srcrect, const SDL_FRect *dstrect,
//                                   double angle, const SDL_FPoint *center,
//                                   SDL_FlipMode flip);

    bool DrawRotatedSrcDstRect(RectF srcRect, RectF dstRect,
            F32 angle, Point2F centerPoint,
            SDL_FlipMode flip = SDL_FLIP_NONE,
            Color color=WHITE) {
        SDL_Texture* tex = mTexture;
        if (color.a < 1 || !tex) return false;
        SDL_SetTextureColorMod(tex, color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(tex, color.a);
        SDL_SetTextureBlendMode(tex, (color.a < 255) ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);

        return SDL_RenderTextureRotated(app.getRenderer(), tex, &srcRect, &dstRect,
                    angle, &centerPoint, flip);
    }
    bool DrawRotatedCentered(F32 x, F32 y, F32 angle, SDL_FlipMode flip = SDL_FLIP_NONE, Color color=WHITE) {
        SDL_Texture* tex = mTexture;
        if (color.a < 1 || !tex) return false;

        F32 w = (F32)tex->w;
        F32 h = (F32)tex->h;
        RectF dstRect = {x - w * 0.5f,y - h * 0.5f, w, h};
        Point2F centerPoint = { w/2, h/2 };

        SDL_SetTextureColorMod(tex, color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(tex, color.a);
        SDL_SetTextureBlendMode(tex, (color.a < 255) ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);

        return SDL_RenderTextureRotated(app.getRenderer(), tex, nullptr, &dstRect,
                    angle, &centerPoint, flip);
    }

    bool DrawSrcDstRect(RectF srcRect, RectF dstRect, Color color=WHITE) {
        SDL_Texture* tex = mTexture;
        if (color.a < 1 || !tex) return false;
        SDL_SetTextureColorMod(tex, color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(tex, color.a);
        SDL_SetTextureBlendMode(tex, (color.a < 255) ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);

        return SDL_RenderTexture(app.getRenderer(), tex, &srcRect, &dstRect);
    }
    bool DrawRect( RectF dstRect, Color color=WHITE) {
        SDL_Texture* tex = mTexture;
        if (color.a < 1 || !tex) return false;
        SDL_SetTextureColorMod(tex, color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(tex, color.a);
        SDL_SetTextureBlendMode(tex, (color.a < 255) ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);

        return SDL_RenderTexture(app.getRenderer(), tex, nullptr, &dstRect);
    }

    bool DrawCentered( F32 x, F32 y, Color color=WHITE) {
        SDL_Texture* tex = mTexture;
        if (color.a < 1 || !tex) return false;

        F32 w = (F32)tex->w;
        F32 h = (F32)tex->h;
        RectF dstRect = {x - w * 0.5f,y - h * 0.5f, w, h};

        SDL_SetTextureColorMod(tex, color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(tex, color.a);
        SDL_SetTextureBlendMode(tex, (color.a < 255) ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);

        return SDL_RenderTexture(app.getRenderer(), tex, nullptr, &dstRect);
    }

};
IMPLEMENT_CONOBJECT(TextureObject);

DefineEngineMethod(TextureObject, getSize,Point2I, (),
                   ,"get the width and height of the loaded texture" ) {
    SDL_Texture* tex = object->get();
    if (!tex) return {0,0};
    return {tex->w,tex->h};
}

DefineEngineMethod(TextureObject, getAtlasRect,RectF, (S32 colCount, S32 rowCount, S32 index),
                   ,"get the rectangle on a atlas texture for a given index" ) {
    return object->getAtlasRect(colCount, rowCount, index);
}


DefineEngineMethod(TextureObject, DrawRotatedSrcDstRect,bool,
        (RectF srcRect, RectF dstRect,F32 angle, Point2F centerPoint, S32 sdl_flip,  Color color),
        (0,WHITE)
        ,"Draw a rotated and optional flipped texture with source and destination rect."
        "@flip: see also SDL_FLIP_ constants" ) {
    return object->DrawRotatedSrcDstRect(srcRect, dstRect,angle, centerPoint, (SDL_FlipMode)sdl_flip, color);
}

DefineEngineMethod(TextureObject, DrawRect,bool, (RectF dstRect, Color color), (WHITE)
                   ,"Draw a texture with source and destination rect" ) {
     return object->DrawRect( dstRect, color);
}

DefineEngineMethod(TextureObject, DrawSrcDstRect,bool,
                   (RectF srcRect, RectF dstRect,  Color color),
                   (WHITE)
                   ,"Draw a the texture from srcRect to dstRect") {
    return object->DrawSrcDstRect(srcRect, dstRect, color);

}



DefineEngineMethod(TextureObject, DrawCentered,bool, (F32 x, F32 y, Color color), (WHITE)
                   ,"Draw the texture centered at the position" ) {
    return object->DrawCentered(x,y,color);
}


DefineEngineMethod(TextureObject, DrawRotatedCentered,bool, (F32 x, F32 y, F32 angle , S32 flip, Color color)
    , (0,WHITE)
    ,"Draw a centered rotated (optional flipped) texture at the position"
    "@flip: see also SDL_FLIP_ constants" ) {
    return object->DrawRotatedCentered(x,y,angle, (SDL_FlipMode)flip, color);
}



// =============================================================================
//  GameObject
// =============================================================================
class GameObject :public SimObject
{
    typedef SimObject Parent;
public:
    DECLARE_CONOBJECT(GameObject);
    Point3F mPoint = {0.f,0.f,0.f};
    Point3F mSize = {0.f,0.f,0.f};
    Point3F mVelo = {0.f,0.f,0.f};
    Color mColor = {0,0,0, 0};

    F32   mRotateAngle = 0.f;
    S32   mFlip = 0; //SDL_FlipMode


    static void initPersistFields() {
          Parent::initPersistFields();
        addField("position", TypePoint3F, Offset(mPoint,GameObject));
        addField("x", TypeF32, Offset(mPoint.x,GameObject));
        addField("y", TypeF32, Offset(mPoint.y,GameObject));
        addField("z", TypeF32, Offset(mPoint.z,GameObject));

        addField("velocity", TypePoint3F, Offset(mVelo,GameObject));
        addField("veloX", TypeF32, Offset(mVelo.x,GameObject));
        addField("veloY", TypeF32, Offset(mVelo.y,GameObject));
        addField("veloZ", TypeF32, Offset(mVelo.z,GameObject));

        addField("size", TypePoint2F, Offset(mSize.x,GameObject));
        addField("width", TypeF32, Offset(mSize.x,GameObject));
        addField("heigth", TypeF32, Offset(mSize.y,GameObject));

        addField("color", TypeColor, Offset(mColor,GameObject));
        addField("r", TypeS8, Offset(mColor.r,GameObject));
        addField("g", TypeS8, Offset(mColor.g,GameObject));
        addField("b", TypeS8, Offset(mColor.b,GameObject));
        addField("a", TypeS8, Offset(mColor.g,GameObject));


        addField("rotate", TypeF32, Offset(mRotateAngle,GameObject), "angle in degree");
        addField("flip", TypeS32, Offset(mFlip,GameObject), "is flipped (SDL_FlipMode)");
    }

    RectF getRectF() {
       return {mPoint.x, mPoint.y, mSize.x, mSize.y};
    }
    Point2F getCenter2F() {
        return {mPoint.x + mSize.x / 2, mPoint.y + mSize.y / 2.f};
    }

}; //CLASS
IMPLEMENT_CONOBJECT(GameObject);

DefineEngineMethod(GameObject, getRectF, RectF, (), ,"get the current rect") {
    return object->getRectF();
}
DefineEngineMethod(GameObject, getCenter2F, Point2F, (), ,"get the center point") {
    return object->getCenter2F();
}
DefineEngineMethod(GameObject, moveLinear, void, (), ,"Move Linear position/velocity") {
    F32 dt =  (F32)BaseFlux::getFrameTime();
    object->mPoint.x += object->mVelo.x * dt;
    object->mPoint.y += object->mVelo.y * dt;
    object->mPoint.z += object->mVelo.z * dt;
}

DefineEngineMethod(GameObject, moveGravity, void, (F32 gravityX, F32 gravityY, F32 gravityZ),
                        (0.f, 9.81f, 0.f) ,
                    "Move with gravity acceleration default: 0,-9.81,0") {
    F32 dt = (F32)BaseFlux::getFrameTime();

    object->mVelo.x += gravityX * dt;
    object->mVelo.y += gravityY * dt;
    object->mVelo.z += gravityZ * dt;

    object->mPoint.x += object->mVelo.x * dt;
    object->mPoint.y += object->mVelo.y * dt;
    object->mPoint.z += object->mVelo.z * dt;
}

DefineEngineMethod(GameObject, moveOrbital, void, (Point3F centerPoint, F32 centerMass), ,"Move in a gravitational field") {
    F32 dt = (F32)BaseFlux::getFrameTime();

    Point3F direction = centerPoint - object->mPoint;
    F32 distance = direction.len();

    F32 softening = 0.1f;

    if (distance > 0.00001f) {
        direction.normalize();

        F32 G = 6.674f;

        F32 gravityPull = (G * centerMass) / (distance * distance + softening);

        object->mVelo.x += direction.x * gravityPull * dt;
        object->mVelo.y += direction.y * gravityPull * dt;
        object->mVelo.z += direction.z * gravityPull * dt;
    }

    object->mPoint.x += object->mVelo.x * dt;
    object->mPoint.y += object->mVelo.y * dt;
    object->mPoint.z += object->mVelo.z * dt;
}

DefineEngineMethod(GameObject, moveOrbital2D, void,
    (Point3F centerPoint, F32 gravity, F32 softening, F32 maxSpeed),
                   (10.f, 150.f, 350.f) ,"2D Safe Orbital Movement")
{
    F32 dt = (F32)BaseFlux::getFrameTime();

    Point3F direction = centerPoint - object->mPoint;
    direction.z = 0.f;

    F32 distance = direction.len();

    // F32 G = 9.81f;

    if (distance > 0.0001f) {
        direction.normalize();
        F32 gravityPull = (gravity * 1000.f) / (distance * distance + softening);

        object->mVelo.x += direction.x * gravityPull * dt;
        object->mVelo.y += direction.y * gravityPull * dt;
    }

    // F32 drag = 0.995f;
    // object->mVelo.x *= drag;
    // object->mVelo.y *= drag;

    F32 currentSpeed = sqrt(object->mVelo.x * object->mVelo.x + object->mVelo.y * object->mVelo.y);
    if (currentSpeed > maxSpeed && currentSpeed > 0.0f) {
        object->mVelo.x = (object->mVelo.x / currentSpeed) * maxSpeed;
        object->mVelo.y = (object->mVelo.y / currentSpeed) * maxSpeed;
    }

    object->mPoint.x += object->mVelo.x * dt;
    object->mPoint.y += object->mVelo.y * dt;
}


DefineEngineMethod(GameObject, DrawTexture, bool, (SimObjectId texObjectID, bool center),(true) ,"Draw2D") {
    TextureObject* texObject = dynamic_cast<TextureObject*>(Sim::findObject(texObjectID));
    if (!texObject) return false;
    if (object->mSize.x <= 0.f || object->mSize.y <= 0.f ) {
        object->mSize.x = (F32)texObject->get()->w;
        object->mSize.y = (F32)texObject->get()->h;
    }
    RectF rect;
    if (center) rect = {
        object->mPoint.x - object->mSize.x / 2.f,object->mPoint.y - object->mSize.y / 2.f
        , object->mSize.x, object->mSize.y };
    else rect = { object->mPoint.x,object->mPoint.y, object->mSize.x, object->mSize.y };

    if (object->mRotateAngle != 0.f || object->mFlip != 0) {
        RectF srcRect = {0.f,0.f,(F32)texObject->get()->w, (F32)texObject->get()->h};
        return texObject->DrawRotatedSrcDstRect(srcRect, rect,
                object->mRotateAngle, {object->mSize.x / 2.f, object->mSize.y / 2.f },
                (SDL_FlipMode)object->mFlip, object->mColor);
    }
    return texObject->DrawRect( rect , object->mColor);
}

DefineEngineMethod(GameObject, solveCollideLine,bool, (RectF linePoints, F32 bounceStrength)
        ,(0.2f),"check collision agains a line .. using Points as parameter x1,x2,y1,y2 (packed in a RectF)")
{
    BaseFlux::Collision::Info info;
    RectF myRect = object->getRectF();
    if ( BaseFlux::Collision::getInfoRectLine(myRect
      , linePoints.x, linePoints.y, linePoints.w, linePoints.h
      , info)) {

        BaseFlux::Collision::solveOberlap(myRect, info);
        object->mPoint.x = myRect.x;
        object->mPoint.y = myRect.y;

///
        if (bounceStrength > 0.f) {
            float dotProduct = (object->mVelo.x * info.mNormal.x) + (object->mVelo.y * info.mNormal.y);
            if (dotProduct < 0.0f) {
                float impulse = -(1.0f + bounceStrength) * dotProduct;
                object->mVelo.x += info.mNormal.x * impulse;
                object->mVelo.y += info.mNormal.y * impulse;
            }
        }
///

        return true;
    }
    return false;
}


DefineEngineMethod(GameObject, getCollideRectF,bool, (SimObjectId gameObjectID, bool alsoNullVelo)
    ,(false),"Check collide and move out")
{
    if (object->mSize.x <= 0.f || object->mSize.y <= 0.f ) return false; //invalid size!
    GameObject* other = dynamic_cast<GameObject*>(Sim::findObject(gameObjectID));
    if (!other) return false;
    BaseFlux::Collision::Info info;
    RectF myRect = object->getRectF();
    if (BaseFlux::Collision::getInfoRectF(myRect, other->getRectF(), info)) {
        BaseFlux::Collision::solveOberlap(myRect, info);
        object->mPoint.x = myRect.x;
        object->mPoint.y = myRect.y;
        if (alsoNullVelo) { //else let the script decide
            object->mVelo.x = 0.f;
            object->mVelo.y = 0.f;
        }
        return true;
    }
    return false;
}

// =============================================================================
//  SoundObject
// =============================================================================
class SoundObject : public SimObject
{
    typedef SimObject Parent;
    StringTableEntry mFileName=nullptr;
    bool mLoop = false;
    F32 mGain = 1.f;
    BaseFlux::WavData* mWaveData;
public:
    DECLARE_CONOBJECT(SoundObject);
    SoundObject() {
        mFileName = StringTable->EmptyString();
    }
    bool onAdd() override {
        mWaveData = app.getSound(mFileName);
        if (!mWaveData) return false;
        return Parent::onAdd();
    }
    // void onRemove() override;

    BaseFlux::WavData* get() { return mWaveData; };
    static void initPersistFields() {
          Parent::initPersistFields();
        addField("fileName", TypeString, Offset(mFileName,SoundObject));
        addField("gain", TypeF32, Offset(mGain,SoundObject));
        addField("loop", TypeBool, Offset(mLoop,SoundObject));
    }

    bool play() {
        return app.getAudioManager().play(mWaveData, mGain, mLoop);
    }
};
IMPLEMENT_CONOBJECT(SoundObject);

DefineEngineMethod(SoundObject, play,bool, (),
                   ,"play the sound" ) {
    return object->play();
}

// =============================================================================
ConsoleFunctionGroupBegin( SDL, "SDL/BaseFlux functions");
// =============================================================================

DefineEngineFunction(ClearBackground, void , (Color color),
                     ,"Set the default background color - one call is enough") {
   app.getSettings().clearColor = color;

}


// -----------------------------------------------------------------------------
// SDL
// -----------------------------------------------------------------------------
DefineEngineFunction(SetColor, void , (Color color),
                     ,"set the render color") {
    SDL_SetRenderDrawColor(app.getRenderer(), color.r, color.g, color.b, color.a);
}
// ----------------------------------------------------------------------------
DefineEngineFunction(BeginBlendMode, void, (),,"") {
    SDL_SetRenderDrawBlendMode(app.getRenderer(), SDL_BLENDMODE_BLEND );

}
DefineEngineFunction(EndBlendMode, void,(),,"") {
    SDL_SetRenderDrawBlendMode(app.getRenderer(), SDL_BLENDMODE_NONE );
}
// ----------------------------------------------------------------------------
DefineEngineFunction(BeginScale, void , (F32 x, F32 y),,"") {
    SDL_SetRenderScale(app.getRenderer(), x, y);
}
DefineEngineFunction(EndScale, void , (),,"") {
    SDL_SetRenderScale(app.getRenderer(), 1.f, 1.f);
}
// -----------------------------------------------------------------------------
DefineEngineFunction(PointInRect, bool , (Point2I p, RectI rect),
                     ,"Check a point is in rect") {
    return (bool)SDL_PointInRect(&p, &rect);
}
DefineEngineFunction(PointInRectF, bool , (Point2F p, RectF rect),
                     ,"Check a point is in rect") {
    return (bool)SDL_PointInRectFloat(&p, &rect);
}
DefineEngineFunction(HasRectIntersection, bool , (RectI rectA, RectI rectB),
                     ,"Check rect intersection") {
    return (bool)SDL_HasRectIntersection(&rectA, &rectB);
}

DefineEngineFunction(HasRectIntersectionF, bool , (RectF rectA, RectF rectB),
                     ,"Check rect intersection") {
    return (bool)SDL_HasRectIntersectionFloat(&rectA, &rectB);
}
DefineEngineFunction(GetRectIntersectionF, RectF , (RectF rectA, RectF rectB),
                     ,"get rect intersection (overlap)") {
    RectF result = {0.f,0.f,0.f};
    SDL_GetRectIntersectionFloat(&rectA, &rectB, &result);
    return result;
}
DefineEngineFunction(GetRectUnionF, RectF , (RectF rectA, RectF rectB),
                     ,"get rect unio both rects combined to one big.") {
    RectF result = {0.f,0.f,0.f};
    SDL_GetRectUnionFloat(&rectA, &rectB, &result);
    return result;
}
// extern SDL_DECLSPEC bool SDLCALL SDL_GetRectEnclosingPointsFloat(const SDL_FPoint *points, int count, const SDL_FRect *clip, SDL_FRect *result);

DefineEngineFunction(HasRectLineIntersectionF, bool , (RectF rect, F32 x1,F32 y1, F32 x2, F32 y2),
                     ,"check if a rect and a line intersects") {

    return SDL_GetRectAndLineIntersectionFloat(&rect, &x1, &y1, &x2, &y2);
}



// ------

DefineEngineFunction(DrawFPS, void, (F32 x, F32 y)
,, "Draw FPS as position")
{
    Color color = GREEN;
    U32 fps =  BaseFlux::getFPS();
    String text = String::ToString("%d fps",fps);

    if (fps < 15) color = RED;
    else if (fps < 30) color = ORANGE;

    BaseFlux::DrawDebugText(app.getRenderer(),x,y,text.c_str(), 1.0f, color, true/*, shadowColor*/);
}

DefineEngineFunction(DrawText, void, ( F32 x, F32 y,String text,
                                      F32 scale, Color color,
                                      bool doShadow, Color shadowColor)
,(1.0, BLACK, false, DARKGRAY),"Draw a Text with optional shadow")
{
    BaseFlux::DrawDebugText(app.getRenderer(),x,y,text.c_str(), scale, color, doShadow, shadowColor);
}



DefineEngineFunction(DrawLine, void, (F32 x1, F32 y1,F32 x2, F32 y2, Color color)
        ,(WHITE),"Draw a Line") {
    BaseFlux::DrawLine(app.getRenderer(), Point2F(x1,y1),Point2F(x2,y2), color);
}

DefineEngineFunction(DrawLineRec, void, (RectF points, Color color)
,(WHITE),"Draw a Line using rect w=x2 h=y2 as parameter") {
    BaseFlux::DrawLine(app.getRenderer(), Point2F(points.x,points.y),Point2F(points.w,points.h), color);
}

DefineEngineFunction(DrawRect, void, (F32 x, F32 y,F32 w, F32 h, Color color, bool fill)
        ,(WHITE, true),"Draw a Rect") {
    BaseFlux::DrawRect(app.getRenderer(), RectF(x,y,w,h), color, fill);
}
DefineEngineFunction(DrawRectRec, void, (RectF rect, Color color, bool fill)
,(WHITE, true),"Draw a Rect") {
    BaseFlux::DrawRect(app.getRenderer(), rect, color, fill);
}

DefineEngineFunction(DrawCircle, void, (F32 x, F32 y,F32 radius, Color color, bool fill)
    ,(WHITE, true),"Draw a Circle") {
    BaseFlux::DrawCircle(app.getRenderer(), radius, Point2F(x,y), color, fill);
}
DefineEngineFunction(DrawArc, void, (F32 x, F32 y,F32 radius,F32 startRad, F32 endRad, Color color, bool fill)
,(WHITE, true),"Draw a Arc: startRad / endRad in radians ") {
    BaseFlux::DrawArc(app.getRenderer(), radius,startRad, endRad, Point2F(x,y), color, fill);
}
DefineEngineFunction(DrawDonut, void, (F32 x, F32 y,F32 innerRadius,F32 outerRadius, Color color, bool fill)
,(WHITE, true),"Draw a Arc") {
    BaseFlux::DrawDonut(app.getRenderer(), innerRadius,outerRadius, Point2F(x,y), color, fill);
}


// -----------------------------------------------------------------------------
ConsoleFunctionGroupEnd(SDL);

// -----------------------------------------------------------------------------
// this is ported from OmFlux/KorkTest to OmFlux/ElfTest to baseElf
// since it was inital written for KorkTest it use old style ConsoleFunction
// -----------------------------------------------------------------------------
ConsoleFunctionGroupBegin(BaseFlux, "BaseFlux Functions: getFPS, ...");

ConsoleFunction(getFullScreen, bool, 1,1, "") {
    Uint32 flags = SDL_GetWindowFlags(app.getWindow());
    return (flags & SDL_WINDOW_FULLSCREEN);
}
DefineEngineFunction(setFullScreen, bool,(bool value),, "bool value") {
    return SDL_SetWindowFullscreen(app.getWindow(),value);
}

ConsoleFunction(getGameTime, F32, 1,1, "") {
    return (F32) BaseFlux::getGameTime();
}

ConsoleFunction(getFrameTime, F32, 1,1, "") {
    return (F32) BaseFlux::getFrameTime();
}



ConsoleFunction(getRealTime, S32, 1,1, "") {
    return Sim::getCurrentTime();
}

ConsoleFunction(getFPS, S32, 1,1, "") {
    return (S32)BaseFlux::getFPS();
}
DefineEngineFunction(GetMousePosition, Point2I, (),, "") {
    return gMousePos;
}


DefineEngineFunction(setWindowSize, bool, (S32 x, S32 y), , "") {
   return  SDL_SetWindowSize(app.getWindow(), x, y);
}
DefineEngineFunction(getWindowSize, Point2I, (), , "") {
    int x, y;
    SDL_GetWindowSize(app.getWindow(), &x, &y);
    return {x,y};
}

DefineEngineFunction(setVSync, void, (bool value), , "bool value") {
    SDL_SetRenderVSync(app.getRenderer(), (int)value);
}

DefineEngineFunction(getFullPath, String,(),, "get the current directory") {
    return Torque::FS::GetCwd().getFullPath();
}

// ----------------- include = exec with nocalls ----------------------

DefineEngineFunction(include,bool, (String fileName),, "include(fileName)" "exec a file without calls" ){
    return Con::executeFile(fileName, true);
}
// ----------------- debuglog ----------------------
//-----------------------------------------------------------------------------

DefineEngineStringlyVariadicFunction( dEcho, void, 2, 0, "debug echo ( string message... ) ")
{
    #ifdef TORQUE_DEBUG
    U32 len = 0;
    S32 i;
    for(i = 1; i < argc; i++)
        len += dStrlen(argv[i]);

    char *ret = Con::getReturnBuffer(len + 1);
    ret[0] = 0;
    for(i = 1; i < argc; i++)
        dStrcat(ret, argv[i], (U64)(len + 1));

    Con::printf("%s", ret);
    ret[0] = 0;
    #endif
}

//-----------------------------------------------------------------------------

DefineEngineStringlyVariadicFunction( dWarn, void, 2, 0, "debug warn( string message... ) " )
{
    #ifdef TORQUE_DEBUG
    U32 len = 0;
    S32 i;
    for(i = 1; i < argc; i++)
        len += dStrlen(argv[i]);

    char *ret = Con::getReturnBuffer(len + 1);
    ret[0] = 0;
    for(i = 1; i < argc; i++)
        dStrcat(ret, argv[i], (U64)(len + 1));

    Con::warnf(ConsoleLogEntry::General, "%s", ret);
    ret[0] = 0;
    #endif
}

//-----------------------------------------------------------------------------

DefineEngineStringlyVariadicFunction( dError, void, 2, 0, "(debug error  string message... ) ")
{
    #ifdef TORQUE_DEBUG
    U32 len = 0;
    S32 i;
    for(i = 1; i < argc; i++)
        len += dStrlen(argv[i]);

    char *ret = Con::getReturnBuffer(len + 1);
    ret[0] = 0;
    for(i = 1; i < argc; i++)
        dStrcat(ret, argv[i], (U64)(len + 1));

    Con::errorf(ConsoleLogEntry::General, "%s", ret);
    ret[0] = 0;
    #endif
}
//-----------------------------------------------------------------------------
ConsoleFunctionGroupEnd(BaseFlux);
// -----------------------------------------------------------------------------
// added at bottom
void InitBindings_SDL() {

    Con::registerEnumS32<SDL_ScaleMode>("", false);
    Con::registerEnumS32<SDL_FlipMode>("", false);
    registerColors();

    gSettingsObject = new SettingsObject();
    gSettingsObject->registerObject(); //make available on Console
    Con::setIntVariable("$Settings", gSettingsObject->getId());
    // -----
}
void ShutdownBindings_SDL() {
    Con::setIntVariable("$Settings", 0);
    gSettingsObject->deleteObject();
}
//-----------------------------------------------------------------------------
