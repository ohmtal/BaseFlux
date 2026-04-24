
function test()
    print("TEST!")
end

local function MyEvent(ev)
--     print(string.format("LUA: SDL event: %d", ev.type))
    if ev.type == SDL_EVENT_QUIT then
        print("will quit")
    end

    if (ev.type == SDL_EVENT_KEY_DOWN)  then
        local key = ev.key.key
--         print(string.format("LUA: SDL keydown: %d", key))
        if key == sdl.K_SPACE then
            print("space")
        elseif key == sdl.K_W then
            print("run forest")
        elseif key == sdl.K_ESCAPE then
            print("escape fired")
        end
    end
end


local backtex = getTexture("back.bmp")

local function MyRenderer()
--      renderTexture("back.bmp")
     renderTexture(backtex)
end



OnSDLEvent(MyEvent)
OnRender(MyRenderer)


local function MyUpdate(delta)
-- FIXME NO idea why delta is nil !!!!!!
--     print("DT:", delta)
end
OnUpdate(MyUpdate)
-- OnUpdate( function(delta)
--       print("DT:", delta)
-- end )



print("Lua-Skript loaded")


