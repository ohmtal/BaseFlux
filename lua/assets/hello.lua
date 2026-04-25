-- Hello World in rainbow color ;)

local textColor = { r = 255, g = 255, b = 255}
local timer = 0

-- ---------- OnEvent --------------
OnSDLEvent( function(ev)
    if (ev.type == SDL_EVENT_KEY_DOWN)  then
        local key = ev.key.key
        if key == sdl.K_ESCAPE then
            quit();
        end
    end

end )
-- ---------- OnRender --------------
OnRender( function()
    setScale(2.0,2.0)
    setColor( textColor.r, textColor.g, textColor.b)
    drawDebugText(20, 20, "Hello World!")
    setScale(1.0,1.0)


end )
-- ---------- OnUpdate --------------
OnUpdate(function(dt)
    timer = timer + dt

    textColor = {
        r = math.floor(math.sin(timer) * 127 + 128),
        g = math.floor(math.sin(timer + 2) * 127 + 128),
        b = math.floor(math.sin(timer + 4) * 127 + 128)
    }
end)
