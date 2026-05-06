#include "raylib.h"
#include "raymath.h"
#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui_custom.h"

#include <string>
#include <functional>
#include <math.h>

//--------------------------------------------------------------------------
#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif

#define COLOR_SLATEGRAY   CLITERAL(Color){ 20, 20, 20, 255 }      // Darkest Gray
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
    std::string GetUserConfigPath() {
        char path[512] = { 0 };

        #if defined(_WIN32)
        const char* appData = getenv("APPDATA");
        if (appData) snprintf(path, sizeof(path), "%s", appData);

        #elif defined(__ANDROID__)
        snprintf(path, sizeof(path), "%s", GetAndroidAppDirectory());

        #elif defined(__EMSCRIPTEN__)
        snprintf(path, sizeof(path), "%s",  "/home/web_user");

        #elif defined(__unix__)
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
        if (base.empty()) return std::string();
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
    bool EnableFSAA       = true;

    std::string Company = "RayLib Test Company";
    std::string Caption = "RayLib Caption";
    std::string Version = "RayLib Test 0.260505";

    //FIXME
//     // your window icon (have to be .bmp or .png)
//     std::string IconFilename = "";
//
//     //pre path for IconFilename and loadTexture
//     // base:/ is replaced with your BasePath
//     // NOTE: add a tralling slash!
//
//     std::string AssetPath = "base:/assets/";
//     std::string SoundPathAppend   = "";
//     std::string TexturePathAppend = "";
//
//     //imgui
//     bool EnableDockSpace = true;
//     // pref:/ is replaced with your pref Path
//     std::string IniFileName = "pref:/appgui.ini";

    // overwrite Window FLAGS:
    int windowFlagsOverwrite = 0;

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
// int Log(const char *szFormat, va_list args)
// {
//     if (!szFormat) return -1;
//     TraceLog(LOG_INFO, szFormat, args );
//
//     return 0;
// }
//------------------------------------------------------------------------------
// ---------------- Main Class
class RayFlux  {

public:
    Settings settings;
    std::function<void()> OnRender = nullptr;
    std::function<void(const float)> OnUpdate = nullptr;
    // std::function<void(const SDL_Event)> OnEvent = nullptr;
    std::function<void()> OnShutDown = nullptr;

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
            if (settings.EnableFSAA) windowFlags |= FLAG_MSAA_4X_HINT;
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
            // fixme fixed update / deltaTime
            if (OnUpdate) OnUpdate(0.166);

            BeginDrawing();
            ClearBackground(COLOR_SLATEGRAY);
            if (OnRender) OnRender();
            EndDrawing();
        }
        if (OnShutDown) OnShutDown();
         CloseWindow();
    }
}; //class
//------------------------------------------------------------------------------
struct LazyGui {
   U16 x = 0;
   U16 y = 0;
   U16 size = 20;


   U16 spacing = 10;
   void Write(const char* text, Color color = LIGHTGRAY) {
        DrawText(text, x, y,size, color);
        y += size + spacing;
   }

   int CheckBox( const char *text, bool *checked) {
     int result = GuiCheckBox(Rectangle((float) x,(float)y, (float)size, (float)size), text, checked);
     y += size + spacing;
     return result;
   }
   int ComboBox( float width, const char *text, int *active) {
       int result = GuiComboBox(Rectangle((float) x,(float)y, width, (float)size), text, active);
       y += size + spacing;
       return result;
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

    //-------
    // add basic lighting demo .....
    // Define the camera to look into our 3d world
    Camera camera = { 0 };
    camera.position = (Vector3){ 2.0f, 4.0f, 6.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 0.5f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    // Load basic lighting shader
    Shader shader = LoadShader(TextFormat("assets/shaders/glsl%i/lighting.vs", GLSL_VERSION),
                               TextFormat("assets/shaders/glsl%i/lighting.fs", GLSL_VERSION));
    // Get some required shader locations
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    // NOTE: "matModel" location name is automatically assigned on shader loading,
    // no need to get the location again if using that uniform name
    //shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");

    // Ambient light level (some basic lighting)
    int ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, (float[4]){ 0.1f, 0.1f, 0.1f, 1.0f }, SHADER_UNIFORM_VEC4);

    // Create lights
    Light lights[MAX_LIGHTS] = { 0 };
    lights[0] = CreateLight(LIGHT_POINT, (Vector3){ -2, 1, -2 }, Vector3Zero(), YELLOW, shader);
    lights[1] = CreateLight(LIGHT_POINT, (Vector3){ 2, 1, 2 }, Vector3Zero(), RED, shader);
    lights[2] = CreateLight(LIGHT_POINT, (Vector3){ -2, 1, 2 }, Vector3Zero(), GREEN, shader);
    lights[3] = CreateLight(LIGHT_POINT, (Vector3){ 2, 1, -2 }, Vector3Zero(), BLUE, shader);

    // ------- eyes

    Vector2 scleraLeftPosition = { GetScreenWidth() - 200.0f, GetScreenHeight() - 100.f };
    Vector2 scleraRightPosition = { GetScreenWidth() - 60.0f, GetScreenHeight() - 100.f };
    float scleraRadius = 60;

    Vector2 irisLeftPosition = scleraLeftPosition;
    Vector2 irisRightPosition = scleraRightPosition;
    float irisRadius = 24;

    float angle = 0.0f;
    float dx = 0.0f, dy = 0.0f, dxx = 0.0f, dyy = 0.0f;

    //-------
    std::string confPathText = TextFormat("Base Path: %s", Tools::getBasePath().c_str());
    std::string prefPathText = TextFormat("Config Path: %s", app.settings.getPrefsPath().c_str());
    LazyGui lg {10, 10, 20};

    //------
    app.OnUpdate = [&](float dt) {
        UpdateCamera(&camera, CAMERA_ORBITAL);

        // Update the shader with the camera view vector (points towards { 0.0f, 0.0f, 0.0f })
        float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

        // Check key inputs to enable/disable lights
        if (IsKeyPressed(KEY_Y)) { lights[0].enabled = !lights[0].enabled; }
        if (IsKeyPressed(KEY_R)) { lights[1].enabled = !lights[1].enabled; }
        if (IsKeyPressed(KEY_G)) { lights[2].enabled = !lights[2].enabled; }
        if (IsKeyPressed(KEY_B)) { lights[3].enabled = !lights[3].enabled; }

        // Update light values (actually, only enable/disable them)
        for (int i = 0; i < MAX_LIGHTS; i++) UpdateLightValues(shader, lights[i]);


        // Eyes
        //----------------------------------------------------------------------------------
        if (IsWindowResized()) { // FIXME EVENT !!
            // int newWidth = GetScreenWidth();
            // int newHeight = GetScreenHeight();
            scleraLeftPosition = { GetScreenWidth() - 200.0f, GetScreenHeight() - 100.f };
            scleraRightPosition = { GetScreenWidth() - 60.0f, GetScreenHeight() - 100.f };
        }

        irisLeftPosition = GetMousePosition();
        irisRightPosition = GetMousePosition();

        // Check not inside the left eye sclera
        if (!CheckCollisionPointCircle(irisLeftPosition, scleraLeftPosition, scleraRadius - irisRadius))
        {
            dx = irisLeftPosition.x - scleraLeftPosition.x;
            dy = irisLeftPosition.y - scleraLeftPosition.y;

            angle = atan2f(dy, dx);

            dxx = (scleraRadius - irisRadius)*cosf(angle);
            dyy = (scleraRadius - irisRadius)*sinf(angle);

            irisLeftPosition.x = scleraLeftPosition.x + dxx;
            irisLeftPosition.y = scleraLeftPosition.y + dyy;
        }

        // Check not inside the right eye sclera
        if (!CheckCollisionPointCircle(irisRightPosition, scleraRightPosition, scleraRadius - irisRadius))
        {
            dx = irisRightPosition.x - scleraRightPosition.x;
            dy = irisRightPosition.y - scleraRightPosition.y;

            angle = atan2f(dy, dx);

            dxx = (scleraRadius - irisRadius)*cosf(angle);
            dyy = (scleraRadius - irisRadius)*sinf(angle);

            irisRightPosition.x = scleraRightPosition.x + dxx;
            irisRightPosition.y = scleraRightPosition.y + dyy;
        }
    };
    //------
    app.OnRender = [&]() {

        BeginMode3D(camera);
            BeginShaderMode(shader);
            DrawPlane(Vector3Zero(), (Vector2) { 10.0, 10.0 }, WHITE);
            DrawCube(Vector3Zero(), 2.0, 4.0, 2.0, WHITE);
            EndShaderMode();
            // Draw spheres to show where the lights are
            for (int i = 0; i < MAX_LIGHTS; i++)
            {
                if (lights[i].enabled) DrawSphereEx(lights[i].position, 0.2f, 8, 8, lights[i].color);
                else DrawSphereWires(lights[i].position, 0.2f, 8, 8, ColorAlpha(lights[i].color, 0.3f));
            }
            DrawGrid(10, 1.0f);
        EndMode3D();

        // eyes
        DrawCircleV(scleraLeftPosition, scleraRadius, LIGHTGRAY);
        DrawCircleV(irisLeftPosition, irisRadius, BROWN);
        DrawCircleV(irisLeftPosition, 10, BLACK);

        DrawCircleV(scleraRightPosition, scleraRadius, LIGHTGRAY);
        DrawCircleV(irisRightPosition, irisRadius, DARKGREEN);
        DrawCircleV(irisRightPosition, 10, BLACK);


        // debug text
        lg.y = 50;
        lg.size = 40;
        lg.Write("RayLib Test .....", GOLD);
        lg.size = 20;
        lg.Write(TextFormat("FPS: %d", GetFPS()), RED);
        lg.size = 10;
        lg.Write(confPathText.c_str(), ORANGE);
        lg.Write(prefPathText.c_str(), SKYBLUE);
        lg.size = 20;
        lg.Write("LIGHTS:", WHITE);
        lg.CheckBox( "Red", &lights[1].enabled);
        lg.CheckBox( "Green", &lights[2].enabled);
        lg.CheckBox( "Blue", &lights[3].enabled);
        lg.CheckBox( "Yellow", &lights[0].enabled);

        static int testDummy = 0;
        if (lg.ComboBox( 100.f, "FullScreen;Windowed", &testDummy ) == 1) {
            TraceLog(LOG_INFO, "New TestDummy value: %d", testDummy);
        }

    };
    //--------
    app.OnShutDown = [&]() {
        UnloadShader(shader);
    };
    //--------
    app.Execute();

    return 0;
}

