#include <SDL3/SDL.h>
#include "console/engineAPI.h"
#include "console/consoleExtras.h"
#include "ConsoleTypes.h"

#include <string>
#include <format>

#include "BaseFlux/Main.h"
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
void InitBindings() {
    registerColors();
}
// -----------------------------------------------------------------------------
ConsoleFunctionGroupBegin( SDL, "SDL/BaseFlux functions");
// -----------------------------------------------------------------------------

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

DefineEngineFunction(DrawText, void, (F32 x, F32 y, String text,
                    F32 scale, Color color,
                    bool doShadow, Color shadowColor)
                ,(1.0, BLACK, false, DARKGRAY),"Draw a Text with optional shadow")
{
    SDL_Color oldColor;
    F32 oldScaleX, oldScaleY;
    SDL_GetRenderDrawColor(app.getRenderer(),&oldColor.r, &oldColor.g, & oldColor.b, &oldColor.a);
    SDL_GetRenderScale(app.getRenderer(), &oldScaleX, &oldScaleY);
    SDL_SetRenderScale(app.getRenderer(), scale, scale);

    if (doShadow) {
        SDL_SetRenderDrawColor(app.getRenderer(), shadowColor.r,shadowColor.g,shadowColor.b,shadowColor.a);
        SDL_RenderDebugText(app.getRenderer(), (x + 1)/ scale, (y + 1) / scale, text.c_str());
    }
    SDL_SetRenderDrawColor(app.getRenderer(), color.r,color.g,color.b,color.a);
    SDL_RenderDebugText(app.getRenderer(), x / scale, y / scale, text.c_str());

    // restore old scale color
    SDL_SetRenderScale(app.getRenderer(), oldScaleX, oldScaleY);
    SDL_SetRenderDrawColor(app.getRenderer( ), oldColor.r, oldColor.g, oldColor.b, oldColor.a);

}
// -----------------------------------------------------------------------------
ConsoleFunctionGroupEnd(SDL);
// -----------------------------------------------------------------------------
