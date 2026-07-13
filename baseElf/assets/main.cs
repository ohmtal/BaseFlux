// Main Script
$MODULES = "TerrainDemo ModelViewer WaterPlaneDemo gridtest Primitives2D BunnyMark";
// $MODULES = $MODULES SPC "lights_script models_first_person_maze textures_sprite_animation";
// $MODULES = $MODULES SPC "textures_background_scrolling Eyes";
// $MODULES = $MODULES SPC "textures_mouse_painting";
// $MODULES = $MODULES SPC "RenderMap BatchTest";
// $MODULES = $MODULES SPC "DeltaTime shapes_starfield_batch";
// $MODULES = $MODULES SPC "looptest blank shapes_starfield_effect shapes_starfield_nativ";

// LOADFAILTEST shapes_starfield_effect_v1

function Main::init(%this) {
    SetConfigFlags(%this.flags );

    InitWindow(%this.screenWidth, %this.screenHeight, %this.caption);
    SetTargetFPS(%this.currentFps);

    // Image/Icon Test
    // LoadImage return a Image identifier (imageId)
    %this.iconImg = LoadImage("assets/texture/elfFlux_32x32.png");
    SetWindowIcon(%this.iconImg);
    // IconImg can be unloaded now, but i keep it for later testing texture.
    // It's unloaded when programm ends automaticly
    //  UnloadImage(%this.iconImg);

    // %this.fontHackNerd = LoadFontEx("assets/font/HackNerdFontPropo-Regular.ttf", 32, 0, 250);

    %this.ConsoleGui = singleton ConsoleGuiObject();
    %this.ConsoleFont= LoadFontEx("assets/font/JetBrainsMono-Regular.ttf", 16, 0, 250);
    %this.ConsoleGui.setFont( %this.ConsoleFont);
    %this.ConsoleGui.heightDiv = 2; //must be set here !!
    %this.ConsoleGui.fontsize = 16.0; //must be set here !!

    // SetExitKey(0); // i want to use escape to unload module and display a module list
    %this.loadModule();
}

function Main::loadModule(%this, %setNewModuleIndex) {
    if (%setNewModuleIndex !$= "" ) {
        %this.moduleIndex = %setNewModuleIndex;
    }
    %cnt = getWordCount($MODULES);
    if (%this.moduleIndex >= %cnt)  %this.moduleIndex = 0;
    if (%this.moduleIndex < 0 ) %this.moduleIndex = %cnt - 1;

    %this.moduleName = getword($MODULES, %this.moduleIndex);
    echo("---------------------------------------------------------------------");
    echo(" Load Module:" SPC %this.moduleName);
    echo("---------------------------------------------------------------------");

    if (!exec("assets/modules/" @ %this.moduleName @ ".cs")) {
        error("Failed to load module" SPC %this.moduleName);
        return false;
    }

    if (isObject(%this.module)) {
        %this.module.delete();
    }
    // --- old Module unloaded go ahead
    SetTargetFPS(%this.currentFps); //reset fps
    %cmd = %this.getId() @ ".module = Create" @ %this.moduleName @ "();";
    echo("loadModule command is:" SPC %cmd);
    eval(%cmd);
    %created = isObject(%this.module);
    echo("loadModule success:" SPC %created SPC "id:" SPC %this.module.getId());
    if (!%created || !%this.module.isMethod("render")) {
        error("Module failed!! obj:" SPC %this.module SPC "method render exists:" SPC %this.module.isMethod("render"));
        if (%created) %this.module.delete();
        %this.module = 0;
        return false;
    }
    return true;
}

function Main::loop(%this) {

    if ( IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL) ) {
        if (IsKeyPressed(KEY_UP)) {
          %this.moduleIndex--;
          %this.loadModule();
        } else  if (IsKeyPressed(KEY_DOWN)) {
            %this.moduleIndex++;
            %this.loadModule();
        } else if (IsKeyPressed(KEY_R)) {
            rl();
        }
    }


    BeginDrawing();

    if (isObject(%this.module)){
        %this.module.render();
        // echo("HUHU" SPC %this.moduleName);
        DrawText("Loaded module:" SPC %this.moduleName , 5, GetScreenHeight() - 30, 20, BLUE, true, BLACK);
    } else {
        ClearBackground("20 100 100");
        DrawText("Failed to load module:" SPC %this.moduleName, 5, GetScreenHeight() - 30, 20, RED);
    }
    DrawText ("CTRL + UP or DOWN to load an other module. FPS:" SPC GetFPS()
        , 5 , GetScreenHeight() - 50, 20,   BLACK, true, GREEN);

    %this.ConsoleGui.update();
    EndDrawing();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
function MainInit() {
    $isWebBuild = getOS() $= "Emscripten";
    $Main = new ScriptObject() {
        class = "Main";
        // dynamic fields:
        TypeString caption = "elfFlux Demo Loader";
        TypeS32 screenWidth = 1152;
        TypeS32 screenHeight = 648;
        TypeS32 currentFps = 60;
        TypeS32 flags = FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT;
        // TypeS32 flags = $isWebBuild ? 0 :  (FLAG_WINDOW_RESIZABLE);
        TypeS32 moduleIndex = 0;
        TypeS32 module = 0;
    };

    // if ($isWebBuild) $Main.flags = FLAG_WINDOW_HIGHDPI;

    $Main.init();


    return true;
}
function MainShutDown() {
    $Main.delete();
    CloseWindow();        // Close window and OpenGL context
}

function MainLoop() {
    $Main.loop();
}

function rl() { exec("assets/main.cs");}

function updateDocu() { 
    %file = "assets/script_stub.cs";
    if (exportConsoleDocu(%file))
    {
        echo ("Docu exported to:" SPC %file);
        echo ("- In vim you can simple open it alongside the other file to use autocomplete");
        echo (" In VSCode / VSCodium you can install the Script Package (FIXME add URL)");
        echo (" and it should work automaticly as long you have it opened.");
        echo (" FIXME: find out and docu on how to get it work when it's closed.");
    } else {
         error("Failed to export" SPC %file SPC "!");
    }
      
    
}

