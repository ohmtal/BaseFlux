
local function MyEvent(ev)
--     print(string.format("LUA: SDL event: %d", ev.type))
    if ev.type == SDL_EVENT_QUIT then
        print("will quit")
    end
--     print(type(ev.type), type(SDL_EVENT_KEY_DOWN))
    if (ev.type + 0) == (SDL_EVENT_KEY_DOWN + 0) then
        print("Match!");
    end

    if (ev.type == SDL_EVENT_KEY_DOWN)  then
        print ("HELLO i guess the next line cause an exception which i dont see in my log:")
        local key = ev.key.key
        print ("------- NEVER REACHED!")
        print(string.format("LUA: SDL keydown: %d", key))
--
--         print(string.format("LUA: SDL keydown: %d", key))
--         if key == sdl.K_SPACE then
--             print("space")
--         elseif key == sdl.K_W then
--             print("run forest")
--         elseif key == sdl.K_ESCAPE then
--             print("escape fired")
--         end
    end
end

OnSDLEvent(MyEvent)

print("Lua-Skript loaded")


