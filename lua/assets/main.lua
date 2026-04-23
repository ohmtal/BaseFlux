
local function MyEvent(ev)
    print("LUA: SDL event")
    if ev.type == SDL_EVENT_QUIT then
        print("will quit")
    end

    if ev.type == SDL_EVENT_KEY_DOWN then
        local key = ev.key.key

        if key == sdl.K_SPACE then
            print("space")
        elseif key == sdl.K_W then
            print("run forest")
        elseif key == sdl.K_ESCAPE then
            print("escape fired")
        end
    end
end

OnSDLEvent(MyEvent)

print("Lua-Skript loaded")


