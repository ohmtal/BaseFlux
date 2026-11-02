#include <SDL3/SDL.h>
#include "BaseFlux/Main.h"
#include "BaseFlux/Draw.h"

#include "DirtyLittleScript.h"
#include "toolbox/Tools.h" //NOTE: DLS: for log redirect

extern BaseFlux::Main app;
extern SDL_Point gMousePos;
typedef SDL_Color Color;

namespace DLS {
    extern void RegisterMathFunctions();
    extern void RegisterStringFunctions();
}

namespace DLSFlux {

    // =========================================================================
    DLS::VM vm; // let the tape grow ?!
    size_t TapePos = 0;

    DLS::S64 renderEventVarRegister = -1;
    DLS::S64 EventVarRegister = -1;

    void RegisterEvents() {
        renderEventVarRegister = vm.Variables.reserveGlobal(DLS::SymbolTable::insert( "OnRender" ));
    }

    void OnRender() {
        // example:
        // OnRender = function; flux::DrawFPS 50 50 5;;

        if (renderEventVarRegister < 0) return;
        DLS::Value& funcVar = vm.Variables.getGlobal(renderEventVarRegister);
        vm.PlaceTapeAtScriptFunction(funcVar);
        vm.run();
    }


    // =========================================================================
    // NOTE: Test external access to global variables:
    void printVar(std::string varName) {
        DLS::Value value;
        // if you want it fast manually do what getGlobalVariableValueRef does
        // and save the register!
        if (vm.Variables.getGlobalVariableValueRef(varName, value)) {
            DLS::Tools::printf("VAR IS: %s", value.toString().c_str());
        } else {
            DLS::Tools::errorf("Variables %s not found!", varName.c_str());
        }
    }

    // -----------------------------------------------------------------------------
    size_t getTapePos() { return TapePos;}
    //  -----------------------------------------------------------------------------
    bool MapArrayFRect( DLS::Value& value, SDL_FRect& rect) {
        if (value.type != DLS::ValueType::Array) return false;
        auto arr = vm.Variables.getArrayByMasterIndex((size_t)value.getFastLong());
        if (arr.size() < 4) return false;
        rect.x = arr[0].getFloat();
        rect.y = arr[1].getFloat();
        rect.w = arr[2].getFloat();
        rect.h = arr[3].getFloat();
        if (rect.w == 0.f || rect.h == 0.f) return false;
        return true;
    }
    // -----------------------------------------------------------------------------
    bool MapArrayColor( DLS::Value& value, SDL_Color& color) {
        if (value.type != DLS::ValueType::Array) return false;
        auto arr = vm.Variables.getArrayByMasterIndex((size_t)value.getFastLong());
        if (arr.size() < 3) return false;
        color.r = arr[0].getU8();
        color.g = arr[1].getU8();
        color.b = arr[2].getU8();
        if (arr.size() > 3) color.a = arr[3].getU8();
        else color.a = 255;
        return true;

    }

    void RegisterFluxFunctions() {
        using namespace DLS;

        FunctionMap::RegisterFunction("flux::var", [](std::span<Value> args, Value& ret) -> bool {
            if (args.size() < 1) return false;
            printVar(args[0].getString());
            return true;
        }, "print a global by string varname");


        // TODO copied from BaseFlux13
        // FunctionMap::RegisterFunction("flux::size", [](std::span<Value> args, Value& ret) -> bool {
        //     int logiX = 0;
        //     int logiY = 0;
        //
        //     if (args.size() >= 2) {
        //         SDL_Point oldSize = app.getSettings().ScreenSize;
        //         app.getSettings().ScreenSize.x = args[0].getInt();
        //         app.getSettings().ScreenSize.y = args[1].getInt();
        //         if (oldSize.x != app.getSettings().ScreenSize.x
        //             || oldSize.y != app.getSettings().ScreenSize.y) {
        //             SDL_SetWindowSize(app.getWindow(), app.getSettings().ScreenSize.x , app.getSettings().ScreenSize.y);
        //             }
        //
        //             if (args.size() >= 4) {
        //                 logiX = args[2].getInt();
        //                 logiY = args[3].getInt();
        //                 SDL_SetRenderLogicalPresentation(app.getRenderer(), logiX, logiY ,
        //                                                  SDL_LOGICAL_PRESENTATION_STRETCH);
        //                 // SDL_LOGICAL_PRESENTATION_LETTERBOX);
        //             }
        //
        //     }
        //     SDL_RendererLogicalPresentation mode;
        //     SDL_GetRenderLogicalPresentation(app.getRenderer(),&logiX, &logiY, &mode);
        //     auto obj = new VectorValueObject();
        //     obj->mElements.push_back(Value(app.getSettings().ScreenSize.x));
        //     obj->mElements.push_back(Value(app.getSettings().ScreenSize.y));
        //     obj->mElements.push_back(Value(logiX));
        //     obj->mElements.push_back(Value(logiY));
        //     ret = Value (obj);
        //
        //     return true;
        // }, "set / get the screen size and logical size");
        // // ---------------------------------
        // FunctionMap::RegisterFunction("flux::maxfps", [](std::span<Value> args, Value& ret) -> bool {
        //     DirectVariableAccessor vsyncer(&app.getSettings().FpsLimit);
        //     if (args.size() == 1) {
        //         vsyncer.toCpp(args[0]);
        //     }
        //     vsyncer.toValue(ret);
        //     return true;
        // }, "set / get the max fps");
        // // ---------------------------------
        // FunctionMap::RegisterFunction("flux::Close", [](std::span<Value> args, Value& ret) -> bool {
        //     app.TerminateApplication(); return true;
        // }, "Close the Application");
        // // ---------------------------------
        // //     void DrawDebugText( SDL_Renderer *renderer,
        // //                         float x, float y, const char* text,
        // //                         float scale = 1.f, const SDL_Color& color = {100,100,100,255},
        // //                         bool doShadow = false, const SDL_Color& shadowColor = { 60,60,60,255});
        // //
        FunctionMap::RegisterFunction("flux::Text", [](std::span<Value> args, Value& ret) -> bool {
            if (args.size() < 3 || !args[2].isString()) {
                Tools::errorf("flux::Text: Missing Parameter");
                return false;
            }
            float x = args[0].getFloat();
            float y = args[1].getFloat();
            const char* text = args[2].getString().c_str();

            float scale = 1.f;
            SDL_Color color = {200,200,200,255};
            bool doShadow = true;
            SDL_Color shadowcolor = {60,60,60,255};

            if (args.size() > 3)
                scale = args[3].getFloat();

            if (args.size() > 4)
                MapArrayColor(args[4], color);

            if (args.size() > 5)
                doShadow = args[5].getBool();

            if (args.size() > 6)
                MapArrayColor(args[6], shadowcolor);

            BaseFlux::DrawDebugText(app.getRenderer(),x,y,text, scale, color, doShadow, shadowcolor);

            return true;
        }, "Draw a Text on the Screen. x,y,text optional: scale,color,doShadow,shadowColor");
        //
        //
        // // ---------------------------------
        FunctionMap::RegisterFunction("flux::DrawRect", [](std::span<Value> args, Value& ret) -> bool {
            if (args.size() < 3 || !args[0].isArray() || !args[1].isArray()) {
                Tools::errorf("flux::DrawRect: Missing Parameter");
                return false;
            }
            SDL_FRect rect;
            if (!MapArrayFRect(args[0], rect)) return false;
            SDL_Color color;
            if  (!MapArrayColor(args[1], color)) return false;

            bool fill = args[2].getBool();

            BaseFlux::DrawRect(app.getRenderer(),rect, color, fill);
            return true;
        }, "Draw Rectangle Vector 4 floats, Vector 4 int, bool fill");
        // ---------------------------------
        FunctionMap::RegisterFunction("flux::DrawFPS", [](std::span<Value> args, Value& ret) -> bool {
            if (args.size() < 2) return false;

            float x = args[0].getFloat();
            float y = args[1].getFloat();

            float fontSize  = 1.0;
            if (args.size() > 2) fontSize = args[2].getFloat();


            SDL_Color color = {0,255,0,255};
            U32 fps =  BaseFlux::getFPS();

            static std::string fps_text = []{
                std::string s;
                s.reserve(32);
                return s;
            }();
            fps_text.clear();
            std::format_to(std::back_inserter(fps_text), "{} fps", fps);


            if (fps < 15) color = {255,0,0,255};
            else if (fps < 30) color = {200,50,50,255};

            BaseFlux::DrawDebugText(app.getRenderer(),x,y,fps_text.c_str(), fontSize, color, true/*, shadowColor*/);
            return true;
        }, "Draw FPS as position x y ");



    }


    // -------------------------------------------------------------------------
    static void DLS_Logger(DLS::Tools::LogLevel level, const char* message){
        switch (level) {
            case DLS::Tools::LogLevel::warning: SDL_Log("[warn] %s",  message); break;
            case DLS::Tools::LogLevel::error:   SDL_Log("[error] %s",  message); break;
            case DLS::Tools::LogLevel::info:    SDL_Log("[info] %s",  message); break;
            default:SDL_Log("%s",  message); break;
        }
    }

    void Init () {
        DLS::Tools::LogHandler = DLSFlux::DLS_Logger;
        DLS::Init();
        DLS::RegisterMathFunctions();
        DLS::RegisterStringFunctions( );

        RegisterFluxFunctions();
        RegisterEvents();


    }


    bool Run (const std::string& command) {
        using namespace DLS;
        // DLS::Run(command); // NOTE this also works ;) but i want control the VM
        Lexer lex(command);
        std::vector<Token> token = lex.tokenize();
        Parser parser(token,&vm);
        if (!parser.parse() ) {
            return false;
        }
        if (vm.tape.size() == 0) return false;
        vm.run();
        TapePos = vm.tape.size();

        return true;
    }

    bool RunFile(const std::string& fileName, bool ResetTape ) {
        if (ResetTape) {
            vm.tape.clear();
            vm.pc = 0;
        }
        return Run(DLS::loadScript(fileName));
    }

}
