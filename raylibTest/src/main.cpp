#include "raylib.h"
#include <string>
#include <functional>
//--------------------------------------------------------------------------
// ---------------- typedef
// like my defs from TGE ;) U16 is much shorter than "unsigned int"
typedef signed char        S8;      ///< Compiler independent Signed Char
typedef unsigned char      U8;      ///< Compiler independent Unsigned Char
typedef signed short       S16;     ///< Compiler independent Signed 16-bit short
typedef unsigned short     U16;     ///< Compiler independent Unsigned 16-bit short
typedef signed int         S32;     ///< Compiler independent Signed 32-bit integer
typedef unsigned int       U32;     ///< Compiler independent Unsigned 32-bit integer
typedef float              F32;     ///< Compiler independent 32-bit float
typedef double             F64;     ///< Compiler independent 64-bit float
//------------------------------------------------------------------------------
// ---------------- Tools
// For GetUserConfigPath <<
#if defined(__ANDROID__)
#include <android/native_activity.h>
#endif
//--------------------------------------------------------------------------
namespace Tools {
    std::string GetUserConfigPath(void) {
        char path[512] = { 0 };

        #if defined(_WIN32)
        const char* appData = getenv("APPDATA");
        if (appData) snprintf(path, sizeof(path), "%s", appData);

        #elif defined(__ANDROID__)
        snprintf(path, sizeof(path), "%s", GetAndroidAppDirectory());

        #elif defined(__EMSCRIPTEN__)
        snprintf(path, sizeof(path), "%s",  "/home/web_user");

        #elif defined(__linux__)
        const char* home = getenv("HOME");
        if (home) snprintf(path, sizeof(path), "%s/.local/share", home);

        #elif defined(__APPLE__)
        const char* home = getenv("HOME");
        if (home) snprintf(path, sizeof(path), "%s/Library/Application Support", home);
        #endif

        return std::string(path);
    }
    //--------------------------------------------------------------------------
    // "SDL_GetPrefPath" equivalent
    std::string GetPrefPath(const char* org, const char* app) {
        static std::string cachedPath = "";
        if (!cachedPath.empty()) return cachedPath;
        std::string base = GetUserConfigPath();
        if (base.empty()) return std::string(app);
        cachedPath =  base + "/" + org + "/" + app;
        return cachedPath;
    }
    //--------------------------------------------------------------------------
    std::string sanitizeFilenameWithUnderScores(std::string name)
    {
        std::string result;
        for (unsigned char c : name) {
            if (std::isalnum(c)) {
                result += c;
            } else if (std::isspace(c)) {
                result += '_';
            }
            // Special characters (like '.') are ignored/dropped here
        }
        return result;
    }
    //--------------------------------------------------------------------------
    std::string string_replace_all(std::string str, const std::string& from, const std::string& to){
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
        return str;
    }
    //--------------------------------------------------------------------------
    std::string getBasePath() {
        static std::string cachedPath = "";
        if (!cachedPath.empty()) return cachedPath;

        const char* rawPath = GetApplicationDirectory();
        if (rawPath) {
            cachedPath = rawPath;
        }
        return cachedPath;
    }

}

//--------------------------------------------------------------------------
struct Settings {
    U16 ScreenWidth = 1152;
    U16 ScreenHeight = 648;
    // not the exact fps since i use integer and round it.
    U16  FpsLimit = 0;
    bool FullScreen       = false;
    bool WindowMaximized  = false;
    // you also can set FpsLimit
    bool EnableVSync      = true;
    std::string Company = "RayLib Test Company";
    std::string Caption = "RayLib Caption";
    std::string Version = "RayLib Test 0.260505";

    // your window icon (have to be .bmp or .png)
    std::string IconFilename = "";

    //pre path for IconFilename and loadTexture
    // base:/ is replaced with your BasePath
    // NOTE: add a tralling slash!
    std::string AssetPath = "base:/assets/";
    std::string SoundPathAppend   = "";
    std::string TexturePathAppend = "";

    //imgui
    bool EnableDockSpace = true;
    // pref:/ is replaced with your pref Path
    std::string IniFileName = "pref:/appgui.ini";

    // overwrite Window FLAGS:
    int windowFlagsOverwrite = 0;

    // std::string getPrefsPath();
    // std::string getSafeCompany();
    // std::string getSafeCaption();

    //--------------------------------------------------------------------------
    std::string getPrefsPath(){
        static std::string cachedPath = "";
        if (!cachedPath.empty()) return cachedPath;
        cachedPath = Tools::GetPrefPath(getSafeCompany().c_str(), getSafeCaption().c_str());

        return cachedPath;
    }
    //--------------------------------------------------------------------------
    std::string getSafeCompany(){
        return Tools::sanitizeFilenameWithUnderScores(Company);
    }
    //--------------------------------------------------------------------------
    std::string getSafeCaption(){
        return Tools::sanitizeFilenameWithUnderScores(Caption);
    }
    //--------------------------------------------------------------------------
    void setFPSLimit( int Limit) {
         SetTargetFPS(Limit);
         FpsLimit = Limit;
    }
    //--------------------------------------------------------------------------

};
//------------------------------------------------------------------------------
// ---------------- Log Wrapper
int Log(const char *szFormat, va_list args)
{
    if (!szFormat) return -1;
    TraceLog(LOG_INFO, szFormat, args );

    return 0;
}
//------------------------------------------------------------------------------
// ---------------- Main Class
class RayFlux  {

public:
    Settings settings;
    std::function<void()> OnRender = nullptr;

    bool Init() {
        int windowFlags = 0;
        if (settings.windowFlagsOverwrite) windowFlags = settings.windowFlagsOverwrite;
        else {
            if (settings.FullScreen) windowFlags |=  FLAG_FULLSCREEN_MODE;
            else {
                windowFlags |= FLAG_WINDOW_RESIZABLE;
                if (settings.WindowMaximized) windowFlags |= FLAG_WINDOW_MAXIMIZED;
            }
            if (settings.EnableVSync) windowFlags |= FLAG_VSYNC_HINT;
        }

        SetConfigFlags(windowFlags);
        if (settings.FpsLimit) SetTargetFPS(settings.FpsLimit);
        InitWindow(settings.ScreenWidth, settings.ScreenHeight, settings.Caption.c_str());
        if (!IsWindowReady()) return false;

        return true;
    }

    void Execute() {
        while (!WindowShouldClose())
        {
            if (OnRender) OnRender();
        }
        CloseWindow();
    }
}; //class

#define COLOR_SLATEGRAY   CLITERAL(Color){ 20, 20, 20, 255 }      // Darkest Gray

struct LazyText {
   U16 x = 0;
   U16 y = 0;
   U16 size = 20;

   U16 spacing = 10;
   void Write(const char* text, Color color = LIGHTGRAY) {
        DrawText(text, x, y,size, color);
        y += size + spacing;
   }
};
// =============================================================================
int main(void)
{
    RayFlux app;
    app.settings.WindowMaximized = true;
    // app.settings.FullScreen = true;

    if (!app.Init()) {
        TraceLog(LOG_ERROR, "%s", "Failed to Initialize!");
        return 1;
    }
    std::string confPathText = TextFormat("Base Path: %s", Tools::getBasePath().c_str());
    std::string prefPathText = TextFormat("Config Path: %s", app.settings.getPrefsPath().c_str());
    LazyText lt {20, 50, 20};


    app.OnRender = [&]() {
        BeginDrawing();
            ClearBackground(COLOR_SLATEGRAY);
            lt.y = 50;
            lt.size = 40;
            lt.Write("RayLib Test .....", GOLD);
            lt.size = 20;
            lt.Write(TextFormat("FPS: %d", GetFPS()), RED);
            lt.Write(confPathText.c_str(), ORANGE);
            lt.Write(prefPathText.c_str(), SKYBLUE);
        EndDrawing();
    };

    app.Execute();

    return 0;
}

