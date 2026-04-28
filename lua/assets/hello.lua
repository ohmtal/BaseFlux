-- Hello World in rainbow color ;)

local textColor = SDL_Color.new( 255, 255, 255, 255 )
local timer = 0
local scale = 1.0

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

    setColor( textColor.r, textColor.g, textColor.b)
    drawLine( 0 , 30, 400, 30)
--     setScale(2.0,2.0)
--     drawDebugText(20 / 2, 40 / 2, "Hello World!")
--     setScale(1.0,1.0)
    drawText(20,40,"Hello World!", 2.0)

   -- test wrapper which does the same as before but in c++
   setColor(255,0,0) -- test color restore
   drawText(20,60,"Hello World in green", 2.0, sdl.C_GREEN)
   drawText(20,80,"Hello World in red.", 2.0)

   setColor(textColor) --test overload
   drawLine( 0 , 110, 400, 110)
   drawText(20,120,"Hello World in blue.", scale , sdl.C_BLUE)

end )
-- ---------- OnUpdate --------------
OnUpdate(function(dt)
    timer = timer + dt

    textColor.r = math.floor(math.sin(timer) * 127 + 128);
    textColor.g = math.floor(math.sin(timer + 2) * 127 + 128);
    textColor.b = math.floor(math.sin(timer + 4) * 127 + 128);

    if (scale < 2.2) then
        scale = scale + 0.005;
    else
        scale = 1.0
    end
end)
