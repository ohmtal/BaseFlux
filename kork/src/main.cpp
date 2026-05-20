//-----------------------------------------------------------------------------
// ohmFlux KorkScript Testing
// Issues:
//  [ ] myPlatfromProcess needs to be filled
//  [ ] Script parse error when using id like : 1029.dump();
//       ==> Error parsing ("; expected"; token is dump) at 1:5
//       << error is from astGen.processTokens() cause a exception
//-----------------------------------------------------------------------------
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "imgui.h"
#include "BaseFlux/Main.h"
#include "BaseFlux/Tools.h"
#include "spice/gui/ImConsole.h"
// -------
#include "platform/platform.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "sim/simBase.h"
#include "sim/dynamicTypes.h"
#include "core/fileStream.h"
// -------
#include <filesystem>
#include <format>
// -------
#define Log(...) SDL_Log(__VA_ARGS__)
namespace fs = std::filesystem;
// -----------------------------------------------------------------------------
class Player : public SimObject //, public FluxBaseObject
{
    typedef SimObject Parent;

    bool mShielded = false;
    bool mSitting = false;
    S32  mReqUInt = 0;

public:

    // Vector2 mPosition;

    Player()
    {
        // mPosition = {};
    }

    static void initPersistFields()
    {
        Parent::initPersistFields();
        // Does not have the default types like TypeS32 :(
        addField("ReqUInt", TypeReqUInt, Offset(mReqUInt, Player));

        addField("Shielded", TypeBool,     Offset(mShielded, Player)        );
        addField("Sitting",	 TypeBool,     Offset(mSitting, Player)        );

    }

    bool onAdd() override {
        Log("Player %d added.", getId());
        return Parent::onAdd();
    }

    DECLARE_CONOBJECT(Player);
};

IMPLEMENT_CONOBJECT(Player);

ConsoleMethod(Player, jump, void, 2, 2, "")
{
    Log("[info] Player jump :D");
}

//------------------------------------------------------------------------------
void MyLogger(U32 level, const char *consoleLine, void*)
{
    switch (level) {
        case 1: Log("[warn] %s",  consoleLine); break;
        case 2: Log("[error] %s",  consoleLine); break;
        default: Log("%s",  consoleLine); break;
    }

}

//-----------------------------------------------------------------------------
// console redirect ....
void SDLCALL ConsoleLogFunction(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
    if (!userdata) return;
    auto* console = static_cast<ImConsole*>(userdata);
    if (!console) return;

    char lBuffer[1024];
    snprintf(lBuffer, sizeof(lBuffer), "%s", message);
    console->AddLog("%s", message);
}

//-----------------------------------------------------------------------------
class KorkTestBed
{
    BaseFlux::Main mApp;
    ImConsole console;
    std::vector<fs::path> scriptFiles;


    // -------------------------------------------------------------------------
    void OnConsoleCommand(ImConsole* console, const char* command_line) {
        std::string cmdLineStr = command_line;
        // std::string cmd = FluxStr::getWord(cmdLineStr, 0);

        // add (); this may fail on "bla() ;" but i dont care ;)
        if (!cmdLineStr.ends_with(");")) cmdLineStr += "();";

        Con::evaluatef(cmdLineStr.c_str());


    }
    // -------------------------------------------------------------------------


public:
    bool Initialize()
    {
        mApp.getSettings() = {
            .Caption = "KorkFlux"
        };

        if ( !mApp.InitSDL() ) return false;
        mApp.initImGui();

        // Console >>>
        SDL_SetLogOutputFunction(ConsoleLogFunction, &console);
        console.OnCommand = [&](ImConsole* console, const char* command_line) {
            OnConsoleCommand(console, command_line);
        };
        // console <<<


        // korkscript >>>
        Con::init();
        Sim::init();
        Con::addConsumer(MyLogger, NULL);
        Con::evaluatef( R"(
            echo("Testing kork script with OhmFlux ...");
            exec("assets/main.cs");
        )");
        // korkscript <<<<

        fetchScriptFiles();

         mApp.OnEvent = [&](const SDL_Event event) {
             onEvent(event);
         };

        mApp.OnUpdate = [&](const float deltaTime) {
            onUpdate(deltaTime);
        };


        mApp.OnRender = [&](SDL_Renderer* renderer) {
            onDraw(renderer);
        };

        return true;
    }
    //--------------------------------------------------------------------------------------
    void Execute() {
        mApp.Execute();
        Deinitialize();
    }
    //--------------------------------------------------------------------------------------
    void Deinitialize()
    {
        SDL_SetLogOutputFunction(nullptr, nullptr);
    }
    //--------------------------------------------------------------------------------------
    void onEvent(SDL_Event event)
    {

    }
    //--------------------------------------------------------------------------------------
    void onUpdate(const double& dt)
    {
        // advance Torque Time for schedule
        static U32 lastTick = 0;
        Sim::advanceTime(SDL_GetTicks() - lastTick);
        lastTick = SDL_GetTicks();
        // ----
    }
    //--------------------------------------------------------------------------------------
    void onDraw(SDL_Renderer* renderer) {

        static bool showConsole = true;
        static bool showMenu = true;

        if (showMenu) {
            if (ImGui::BeginMainMenuBar()) {
                if (ImGui::BeginMenu("Window")) {
                    ImGui::MenuItem("Main Menu", "F10", &showMenu);
                    ImGui::MenuItem("Console", "GraveAccent", &showConsole);
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Scripts")) {
                    //FIXME current script ?!
                    // if (ImGui::MenuItem(std::format("Hot Reload {}", getScript()).c_str(),"CTRL+R")) {
                    //     LoadScript();
                    // }
                    ImGui::SeparatorText("Files");
                    bool selected = false;
                    for (const auto& f : scriptFiles) {
                        // selected = getScript() == f.string();
                        if (ImGui::MenuItem(f.string().c_str(), nullptr, selected)) {
                            // setScript(f.string());
                            std::string cmd = std::format("exec(\"assets/{}\");", f.string());
                            // Log("Loading with command: %s", cmd.c_str());
                            Con::evaluate(cmd.c_str());
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }
        }

        if (ImGui::IsKeyPressed(ImGuiKey_F10)) showMenu = !showMenu;
        if (ImGui::IsKeyPressed(ImGuiKey_GraveAccent)) showConsole = !showConsole;
        console.Draw("Console",&showConsole);
    };
    //--------------------------------------------------------------------------------------
    bool fetchScriptFiles() {
        std::string path = BaseFlux::Tools::getBasePath() + "assets/";
        scriptFiles.clear();
        try {
            if (fs::exists(path) && fs::is_directory(path)) {
                for (const auto& entry : fs::recursive_directory_iterator(path)) {
                    if (entry.is_regular_file()
                        &&  ( entry.path().extension() == ".cs" || entry.path().extension() == ".tscript")

                    ) {
                        // Full path: luaFiles.push_back(entry.path());
                        scriptFiles.push_back(fs::relative(entry.path(), path));

                    }
                }
            }
        } catch (const fs::filesystem_error& e) {
            Log("[error]%s",e.what());
            return false;
        }
        return true;
    }

};
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------


int main(int argc, char* argv[])
{
    (void)argc; (void)argv;

    KorkTestBed kork;
    if (!kork.Initialize()) return 1;
    kork.Execute();

    return 0;
}


