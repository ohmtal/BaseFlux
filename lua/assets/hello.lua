-- Hello World in rainbow color ;)

local textColor = SDL_Color.new( 255, 255, 255, 255 )
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
    drawDebugText(20 / 2, 20 / 2, "Hello World!")
    setScale(1.0,1.0)

--    test wrapper which does the same as before but in c++
   setColor(255,0,0) -- test color restore
   drawText(20,40,"Hello World again ;)", 2.0, textColor)
   drawText(20,60,"Hello World in red.", 2.0)

end )
-- ---------- OnUpdate --------------
OnUpdate(function(dt)
    timer = timer + dt

    textColor.r = math.floor(math.sin(timer) * 127 + 128);
    textColor.g = math.floor(math.sin(timer + 2) * 127 + 128);
    textColor.b = math.floor(math.sin(timer + 4) * 127 + 128);

end)
