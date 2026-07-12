//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// baseElf Main source
//-----------------------------------------------------------------------------
#include <cstdio>

// ---- ElfScript
#include "main/engineGlue.h"
#include <console/script.h>

// ----  BaseFlux
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "imgui.h"

#include "BaseFlux/Main.h"
#include "spice/gui/ImConsole.h"


BaseFlux::Main app;
ImConsole console;

//-----------------------------------------------------------------------------
// console redirect ....
void SDLCALL ConsoleLogFunction(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
    char lBuffer[1024];
    snprintf(lBuffer, sizeof(lBuffer), "%s", message);
    console.AddLog("%s", message);
}
//------------------------------------------------------------------------------
void MyLogger(U32 level, const char *consoleLine)
{
    switch (level) {
        case 1: SDL_Log("[warn] %s",  consoleLine); break;
        case 2: SDL_Log("[error] %s",  consoleLine); break;
        default: SDL_Log("%s",  consoleLine); break;
    }

}
// ----------------------------------------------------------------------------
void initConsole() {
    SDL_SetLogOutputFunction(ConsoleLogFunction, nullptr);

    console.OnTabCompletion = [&](ImConsole* console, ImGuiInputTextCallbackData* data, bool forward) {
        char buffer[256] = {0};
        snprintf(buffer, sizeof(buffer), "%s", data->Buf);
        U32 newCursorPos = Con::tabComplete(buffer, data->CursorPos, sizeof(buffer) - 1, forward);
        // Log("TESTTAB: %s (%d)", buffer, newCursorPos);

        data->DeleteChars(0, data->BufTextLen);
        data->InsertChars(0, buffer);
        data->CursorPos = newCursorPos;
        data->SelectionStart = newCursorPos;
        data->SelectionEnd = newCursorPos;
        data->BufDirty = true;
    };

    console.OnCommand = [&](ImConsole* console, const char* command_line) {
        Con::evalAutoComplete(command_line);
    };

}
// ----------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    initConsole();
    engineGlue::init(MyLogger, BaseFlux::Tools::getBasePath().c_str());
    Con::executeFile("assets/main.elf");

    app.getSettings() = {
        .ScreenSize = { 800, 450},
        .Caption = "baseElf",
        // .sdlWindowFlagsOverwrite = SDL_WINDOW_BORDERLESS
    };
    if ( !app.InitSDL() ) return 1;
    app.initImGui();

    app.OnRender = [&](SDL_Renderer* renderer) {

        static bool showConsole = true;
        if (ImGui::IsKeyPressed(ImGuiKey_GraveAccent)) showConsole = !showConsole;
        console.Draw("Console",&showConsole);
    };

    app.OnUpdate = [&](const float dt) {
        engineGlue::process((SimTime)(dt * 1000.f));
    };

    app.OnEvent = [&](const SDL_Event& event) {
        if (app.getImGuiIO() && app.getImGuiIO()->WantTextInput) {
            if (event.type == SDL_EVENT_KEY_DOWN ||
                event.type == SDL_EVENT_KEY_UP ||
                event.type == SDL_EVENT_TEXT_INPUT) {
                return;
                }
        }
    };

    app.OnShutDown = [&]() {
        engineGlue::shutDown();
    };

    app.Execute();

    return 0;
}



/*
enum MyEnum {
    None = 0,
    One,
    Two,
    Three
};

bool gShutDownRequest = false;
U64 gFrameTime = 0;
// ----------------------------------------------------------------------------
DefineEngineFunction(getFrameTime, S32, (), , "get the time in ms the last loop did need to finish")
{
    return (S32)gFrameTime;
}

// ----------------------------------------------------------------------------
int argParser(int argc, char* argv[]) {

    gShutDownRequest = true; //default no loop!
    String argStr;
    // argv[0] is program name
    for (int i = 1; i < argc; ++i) {
        if (!argv[i]) continue;
        argStr = argv[i];

        if (argStr.equal("--loop")) {
            gShutDownRequest = false;
            continue;
        }

        // filename test
        if (argStr.equal("--script")) {
            if (i + 1 < argc) {
                String tmpFile = argv[++i];
                Con::infof("Script File test: %s", tmpFile.c_str());
            } else {
                Con::errorf("--script but no file parameter usage: --script myFile.cs");
                return 1;
            }
            continue;
        }

    } //for ...
    return 0;
}
// ----------------------------------------------------------------------------
void MyLogger(U32 level, const char *consoleLine) {
#if defined(__unix__)
    // we use console
    if (!stdConsole) dPrintf("%s\n", consoleLine);
#else
    switch (level) {
        case 1: dPrintf("[warn] %s\n",  consoleLine); break;
        case 2: dPrintf("[error] %s\n",  consoleLine); break;
        default: dPrintf("%s\n",  consoleLine); break;
    }
#endif
}


DefineEngineFunction(HelloWorld, void, (String name), ("fooBar"),""){
    Con::printf("Hello World %s!", name.c_str());
}
// ----------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    printf("Startup ....\n");


    engineGlue::init(MyLogger);
    int ret = argParser(argc, argv);
    if (ret != 0) return ret;

    Con::addVariable("ShutDownRequest", TypeBool, &gShutDownRequest, "");

// moved to addons/shellConsole
#if defined(__unix__)
    // console test:
    StdConsole::create();
    stdConsole->enable(!gShutDownRequest);
    stdConsole->enableInput(!gShutDownRequest);
#endif


    // register enum Test >>
    Con::registerEnumS32<MyEnum>("$MyEnum::", true);
    Con::setScriptConstant("_LEFT_", 1); //real constant using preprocessor
    Con::setScriptConstant("_RIGHT_", 2); //real constant using preprocessor
    // <<<<<


    // filesystem not implemented   Con::setLogMode(0);
    std::string code= R"(
        echo("EnumTest ..................");
        echo($MyEnum::None SPC $MyEnum::One SPC $MyEnum::Two);
        echo("LEFT/RIGHT" SPC _LEFT_ SPC _RIGHT_);
        echo("......................");
        helloWorld("tom");
        error("This is NOT a error!");
        warn("This is NOT a warning!");
        echo( 5 + 5 );

        function FOO::bar(%this) {
            echo(%this.getClassName());
        }
        $foo = new ScriptObject() { class = "FOO"; };
        $foo.userValue = 4711;
        $foo.bar();

        echo("mSin(3.14) =" SPC mSin(3.14));

        schedule(0, 0, "echo", "hello scheduler");

        // -------------------------
        // ... overwrite quit ...
        function quit() {
            $ShutDownRequest = true;
        }
        // -------------------------
        echo("---------------------------------");
        $foo = new ScriptObject();
        $foo.point = "11 666";
        echo("POINT #1:" SPC $foo.point.x SPC $foo.point.y); // 11 666
        $foo.point.y -=  655; //<<< this is ok
        echo("POINT #2:" SPC $foo.point.x SPC $foo.point.y); // 11 11

        $foo.p[1] = "11 666";
        echo("fake point #1:" SPC $foo.p[1].x SPC $foo.p[1].y); // 11 666
        $foo.p[1].y -= 655; //<<< fail!!!
        echo("fake point #2:" SPC $foo.p[1].x SPC $foo.p[1].y); // 11 666 !

        $bar[1] = "11 666";
        $bar[1].y -= 655;
        echo("bar fake var:" SPC $bar[1].x SPC $bar[1].y); // 11 11 !


        $foo.p2 = "11 666";
        echo("fake point #1:" SPC $foo.p2.x SPC $foo.p2.y); // 11 666
        $foo.p2.y -= 655; //<<< fail!!!
        echo("fake point #2:" SPC $foo.p2.x SPC $foo.p2.y); // 11 666 !


    )";
    Con::evaluate(code.c_str(), false, "");

    // argv[0] is not the best way to get it but since i dont have
    // the platform source here and use the placeholder
    // stub/platformProcess.cpp i use it for the demo.
    // not!

    // String dir = "/opt/TorqueScript/ElfScript/demo/";
    // Platform::setMainDotCsDir(dir);
    // Platform::FS::SetCwd(dir);
    // Con::printf("Current script directory: %s (argv[0] %s)",Torque::FS::GetCwd().getFullPath().c_str(), argv[0] );

    Con::executeFile("ElfScript/demo/test.cs");

    bool doMainLoop = Con::isFunction("MainLoop");
    auto start = std::chrono::steady_clock::now();

    // --------- advance time for scheduler this should be placed in the main loop
    while (!gShutDownRequest) {
          engineGlue::process(gFrameTime);

          #if defined(__unix__)
          stdConsole->process();
          #endif

          if (doMainLoop) Con::executef("MainLoop");

          Platform::sleep(16);

          auto end = std::chrono::steady_clock::now();

        // Differenz berechnen (z. B. in Millisekunden)
          auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
          gFrameTime = duration.count();
          start = end;

    }

    // -------- finallize
    engineGlue::shutDown();

    return 0;
}*/
