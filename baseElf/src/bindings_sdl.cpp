//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// ElfScript BaseFlux/SDL3 binding
//-----------------------------------------------------------------------------
#include <SDL3/SDL.h>
#include "console/engineAPI.h"
#include "console/consoleExtras.h"
#include <core/volume.h>

#include <string>
#include <format>

#include "BaseFlux/Main.h"
#include "BaseFlux/Draw.h"
#include <console/script.h>

#include "ConsoleTypes.h"
#include "ColorConstants.h"

extern BaseFlux::Main app;


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
void InitBindings_SDL() {
    registerColors();
}
// -----------------------------------------------------------------------------
// =============================================================================
//  TextureObject
// =============================================================================
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
        addField("fileName", TypeString, Offset(mFileName,TextureObject));
    }
};
IMPLEMENT_CONOBJECT(TextureObject);
DefineEngineMethod(TextureObject, DrawSrcDstRect,bool, (RectF srcRect, RectF dstRect),
                   ,"Draw a texture with source and destination rect" ) {
    return app.getTextureManager().render(object->get(),&srcRect, &dstRect );
}
DefineEngineMethod(TextureObject, DrawRect,bool, (RectF dstRect),
                   ,"Draw a texture with source and destination rect" ) {
    return app.getTextureManager().render(object->get(),nullptr, &dstRect );
}
DefineEngineMethod(TextureObject, Draw,bool, (F32 x, F32 y),
                   ,"Draw a texture with source and destination rect" ) {
    SDL_Texture* tex = object->get();
    if (!tex) return false;
    RectF dstRect = {x,y, (F32)tex->w,(F32)tex->h};
    return app.getTextureManager().render(object->get(),nullptr, &dstRect );
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
DefineEngineFunction(PointInRect, bool , (Point2I p, RectI rect),
                     ,"Check a point is in rect") {
    return (bool)SDL_PointInRect(&p, &rect);
}

DefineEngineFunction(HasRectIntersection, bool , (RectI rectA, RectI rectB),
                     ,"Check rect intersection") {
    return (bool)SDL_HasRectIntersection(&rectA, &rectB);
}

DefineEngineFunction(DrawFPS, void, (F32 x, F32 y)
,, "Draw FPS as position")
{
    Color color = GREEN;
    U32 fps =  BaseFlux::getFPS();
    String text = String::ToString("%d fps",fps);

    if (fps < 35) color = MAROON;
    else if (fps < 15) color = RED;

    BaseFlux::DrawDebugText(app.getRenderer(),x,y,text.c_str(), 1.0f, color, true/*, shadowColor*/);
}

DefineEngineFunction(DrawText, void, (F32 x, F32 y, String text,
                    F32 scale, Color color,
                    bool doShadow, Color shadowColor)
                ,(1.0, BLACK, false, DARKGRAY),"Draw a Text with optional shadow")
{
    BaseFlux::DrawDebugText(app.getRenderer(),x,y,text.c_str(), scale, color, doShadow, shadowColor);
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

ConsoleFunction(getFrameTime, F32, 1,1, "") {
    return (F32) BaseFlux::getFrameTime();
}

ConsoleFunction(getRealTime, S32, 1,1, "") {
    return Sim::getCurrentTime();
}

ConsoleFunction(getFPS, S32, 1,1, "") {
    return (S32)BaseFlux::getFPS();
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
    #ifdef FLUX_DEBUG
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
    #ifdef FLUX_DEBUG
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
    #ifdef FLUX_DEBUG
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
//-----------------------------------------------------------------------------
