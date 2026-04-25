# BaseFlux Lua 

Very basic Lua "Game Engine" you get Events, Drawing, Sound.
I use this for practice lua without have to learn to many command. 

- Console commands
    - load myscript.lua => load a script from assets folder
    - exec myscript.lua => load a script from assets folder (alias for load)
    - rl => hot reload the current script
    - any Lua code

- Events
    - OnRender( function() end )
    - OnUpdate( function( **float** deltaTime) end ) 
    - OnSDLEvent( function( **SDL_Event** event ) )


- data types
    - SDL_FRect( float x, float y, float w, float h)
    - SDL_FPoint( float x, float y)
    - SDL_Texture*
    - SDL_KeyboardEvent( SDL_Keycode key,  SDL_Scancode scancode, bool down )
    - SDL_Event( int type, SDL_KeyboardEvent key, SDL_MouseMotionEvent motion)

- constants 
    - see bindSDLConstants below
    
- texture drawing functions
    - SDL_Texture* getTexture(string fileName)
    - renderTexture(SDL_Texture* texture, SDL_FRect rect)
    - renderTexture(string fileName*, SDL_FRect rect)
    
- primitives drawing functions
    - setColor(byte r, byte g, byte b, [ byte a ])
    - setScale( float x, float y )
    - drawDebugText( float x, float y , string text)
    - drawPoint( float x, float y )
    - drawLine( float x1, float y1, float x2, float y2)
    - drawRect( SDL_FRect rect )
    - drawFillRect( SDL_FRect rect )
    
- sound (wav)
    - playSound( string fileName, [ float gain ], [ bool loop ])
    - stopSound( string fileName 

- App functions
    - quit() - terminate the app
 
- **Notes**
    - SDL_Texture can be a BMP or PNG file
    - Sound
        - fileformat : WAV 
        - current limitation you can play a sound only once at the same time
    - fileName relative to:
        - sounds => assets/sound/
        - textures => assets/texture/
    
- Todo:
    - [ ] bind SDL_Ticks
    - [ ] bind screen resolution and toggle fullscreen
    - [ ] modular so bindings can be used in other projects with demo project without console 
    
```
    void bindSDLConstants(sol::state& lua) {

        lua["SDL_EVENT_QUIT"] = SDL_EVENT_QUIT;
        lua["SDL_EVENT_KEY_DOWN"] = SDL_EVENT_KEY_DOWN;
        lua["SDL_EVENT_KEY_UP"] = SDL_EVENT_KEY_UP;
        lua["SDL_EVENT_MOUSE_MOTION"] = SDL_EVENT_MOUSE_MOTION;
        lua["SDL_EVENT_MOUSE_BUTTON_DOWN"] = SDL_EVENT_MOUSE_BUTTON_DOWN;


        // Keycodes (SDLK_...)
        // Lua: sdl.K_SPACE
        sol::table k = lua.create_named_table("sdl");

        k["K_SPACE"] = SDLK_SPACE;
        k["K_RETURN"] = SDLK_RETURN;
        k["K_ESCAPE"] = SDLK_ESCAPE;
        k["K_BACKSPACE"] = SDLK_BACKSPACE;


        k["K_UP"] = SDLK_UP;
        k["K_DOWN"] = SDLK_DOWN;
        k["K_LEFT"] = SDLK_LEFT;
        k["K_RIGHT"] = SDLK_RIGHT;

        // A-Z
        for (char c = 'a'; c <= 'z'; ++c) {
            std::string name = "K_";
            name += (char)std::toupper(c);
            k[name] = (SDL_Keycode)c;
        }

        //  0-9
        for (char c = '0'; c <= '9'; ++c) {
            std::string name = "K_";
            name += c;
            k[name] = (SDL_Keycode)c;
        }
    }
```
