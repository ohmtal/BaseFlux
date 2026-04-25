-- Status
local gameOver = true;
local score = 0
local hiscore = 0
-- Textures and Sounds
local blacktex = getTexture("black.png")
local whitetex = getTexture("white.png")
local bounceSound = "beep.wav"
local startSound  = "sound1.wav"

--  Wall
local wallThickness = 10
local screenRect = SDL_FRect.new(20, 40, 600, 350)

local walls = {
    top    = SDL_FRect.new(screenRect.x, screenRect.y,  screenRect.w,   wallThickness),
    bottom = SDL_FRect.new(screenRect.x, screenRect.y + screenRect.h - wallThickness,  screenRect.w, wallThickness),
    left   = SDL_FRect.new(screenRect.x, screenRect.y,  wallThickness, screenRect.h),
    right  = SDL_FRect.new( screenRect.x + screenRect.w - wallThickness, screenRect.y , wallThickness, screenRect.h)
}

local borders = { walls.left, walls.top, walls.bottom }
local goals = { left = walls.left, right = walls.right }

-- Ball
local ball = SDL_FRect.new(300, 175, 10, 10)
local ballVel = { x = 150, y = 150 }
local ballSpeed = 1.0

-- paddle
local paddleSize = { w = 5.0 , h = 60.0 }
local paddleVel = 0.0;
local paddleSpeed = 250.0;
local paddle = SDL_FRect.new(screenRect.w - 10.0, (screenRect.h - paddleSize.w) / 2.0, 5.0, 60.0)


-- ---------- OnEvent --------------
OnSDLEvent( function(ev)
    if ev.type == SDL_EVENT_QUIT then
        print("will quit")
    end

    if (ev.type == SDL_EVENT_KEY_DOWN)  then
        local key = ev.key.key
--         print(string.format("LUA: SDL keydown: %d", key))
        if key == sdl.K_SPACE then
            if (gameOver) then
                score = 0
                ballSpeed = 1.0
                gameOver = false;
                playSound(startSound)
            end

        elseif key == sdl.K_UP then
            paddleVel = paddleSpeed * - 1
        elseif key == sdl.K_DOWN then
            paddleVel = paddleSpeed
        elseif key == sdl.K_ESCAPE then
            print("escape fired")
        end
    end

    if (ev.type == SDL_EVENT_KEY_UP)  then
        local key = ev.key.key
        if (key == sdl.K_UP) then
            if ( paddleVel < 0.0 ) then paddleVel = 0.0 end
        elseif  (key == sdl.K_DOWN) then
            if ( paddleVel > 0.0 ) then paddleVel = 0.0 end
        end
    end


end )

-- ---------- OnRender --------------
OnRender( function()
     renderTexture(blacktex, nil, screenRect )
     renderTexture(whitetex, nil, paddle)
     renderTexture(whitetex, nil, ball)

     for _, wall in pairs(borders) do
         renderTexture(whitetex, nil, wall)
     end



     local x = screenRect.x +  screenRect.w + 10
     local y = screenRect.y + wallThickness
     local incY = 20
     setColor(128,128,128)
     drawDebugText(x, y, "BASEFLUX PONG SQUASH LUA EDITION ;)")
     y = y + incY
     drawDebugText(x, y, string.format("Ball POS: %.2f,%.2f VEL: %.2f, %.2f", ball.x, ball.y, ballVel.x, ballVel.y))

     setScale(2.0,2.0)
     setColor(255,255,255)
     y = y + incY
     drawDebugText(x / 2.0, y / 2.0, string.format("SCORE: %2d    HI: %2d", score, hiscore))

     if (gameOver) then
         y = y + incY + 5
         setColor(255,0,0)
         drawDebugText(x / 2.0, y / 2.0, "G A M E   O V E R")
     end
     setScale(1.0,1.0)

end )

-- --------- Collision check -----------
local function checkCollision(a, b)
    return a.x < b.x + b.w and
           a.x + a.w > b.x and
           a.y < b.y + b.h and
           a.y + a.h > b.y
end

-- --------- score up  -----------
local function doScore(  )
    score = score + 1;
    if score > hiscore then hiscore = score end
    ballSpeed = ballSpeed + 0.15;
end
-- ---------- OnUpdate --------------
OnUpdate(function(dt)
--  ball
    if  not gameOver then

        ball.x = ball.x + ballVel.x * ballSpeed * dt
        ball.y = ball.y + ballVel.y * ballSpeed * dt


        for i = 1, #borders do
            if checkCollision(ball, borders[i]) then
                local wall = borders[i]

                if wall == walls.left then
                    ballVel.x = math.abs(ballVel.x)
                    ball.x = wall.x + wall.w
                    doScore()


                elseif wall == walls.top or wall == walls.bottom then
                    ballVel.y = -ballVel.y
                    if wall == walls.top then ball.y = wall.y + wall.h else ball.y = wall.y - ball.h end
                end

                playSound(bounceSound)
            end
        end
    end


    if ball.x > screenRect.w then
        gameOver = true;
        ball.x, ball.y = screenRect.w / 2.0, screenRect.h / 2.0
    end

--  paddle
    paddle.y = paddle.y + paddleVel * dt

    if checkCollision(walls.top, paddle) then
        paddleVel = 0.0
        paddle.y =  walls.top.y + walls.top.h
    elseif checkCollision(walls.bottom, paddle) then
        paddleVel = 0.0
        paddle.y = walls.bottom.y - paddle.h
    end



    if ballVel.x > 0  and not gameOver then
        if checkCollision(ball, paddle) then
            ballVel.x = -ballVel.x

            local paddleCenter = paddle.y + (paddle.h / 2)
            local ballCenter = ball.y + (ball.h / 2)
            ballVel.y = (ballCenter - paddleCenter) * 5.0

            ball.x = paddle.x - ball.w
            playSound(bounceSound)
        end
    end

end)

--- ---------- ------------ ----------
print("Lua-Skript loaded")


