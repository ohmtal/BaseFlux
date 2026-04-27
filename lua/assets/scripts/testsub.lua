local alpha = 255
local keys = {}

OnSDLEvent( function(ev)
    if (ev.type == SDL_EVENT_KEY_DOWN)  then
        keys[ev.key.key] = true
        if (ev.key.key == sdl.K_SPACE) then
            alpha = 255
        end
    end
    if (ev.type == SDL_EVENT_KEY_UP)  then
        keys[ev.key.key] = nil
    end
end )

OnUpdate( function (dt)
   if keys[sdl.K_UP] and alpha < 255 then  alpha =  alpha + 1 end
   if keys[sdl.K_DOWN] and alpha > 0 then  alpha =  alpha - 1 end
end)

OnRender( function()
    setScale(2.0,2.0)
    setColor( 0,0,255, alpha)
    drawDebugText(20, 20, string.format( "Hello from scripts. a=%d", alpha))
    setScale(1.0,1.0)
end )

print("... loaded ...")
