//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// Myst - only hello world was too booring ;) i may make nicer code later ...
//-----------------------------------------------------------------------------
#pragma once
#include "imgui.h"
#include <vector>
#include <deque>

namespace BaseFluxDemo::Mysth {



    inline uint8_t mMaxHistory  = 20;
    inline uint8_t mFrameSkip  =  3;


    struct Point {
        ImVec2 pos, vel;
    };

    struct LineGroup {
        Point p1, p2;
        ImColor color;
        std::deque<std::pair<ImVec2, ImVec2>> history;
    };

    static std::vector<LineGroup> lines(5);
    static bool initialized = false;


    void init( ) {
        for (auto& line : lines) {
            line.p1.pos = ImVec2(100.0f, 100.0f);
            line.p2.pos = ImVec2(200.0f, 200.0f);
            line.p1.vel = ImVec2(2.0f, 1.5f);
            line.p2.vel = ImVec2(-1.2f, 2.2f);
            line.color = ImColor(255, 255, 255, 255);
        }
        initialized = true;
    }


    void RenderBouncingLines() {
        if (!initialized) init();

        ImVec2 winPos = ImGui::GetCursorScreenPos();
        ImVec2 winSize = ImGui::GetContentRegionAvail();
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        static float globalHue = 0.0f;
        globalHue += 0.0005f;
        if (globalHue > 1.0f) globalHue -= 1.0f;

        static int frameSkipCounter = 0;

        for (int i = 0; i < lines.size(); i++) {

            auto& line = lines[i];
            Point* pts[2] = { &line.p1, &line.p2 };


            frameSkipCounter++;

            if ( frameSkipCounter > mFrameSkip ) {
                line.history.push_front({line.p1.pos, line.p2.pos});
                if (line.history.size() > mMaxHistory) line.history.pop_back();
                frameSkipCounter = 0;
            }
            for (size_t i = 0; i < line.history.size(); i++) {
                float alpha = 1.0f - ((float)i / mMaxHistory);
                ImColor trailColor = line.color;
                trailColor.Value.w *= alpha;

                auto& oldPos = line.history[i];
                drawList->AddLine(
                    ImVec2(winPos.x + oldPos.first.x, winPos.y + oldPos.first.y),
                                  ImVec2(winPos.x + oldPos.second.x, winPos.y + oldPos.second.y),
                                  trailColor, 1.5f
                );
            }

            float lineHue = globalHue + (i * 0.05f);
            if (lineHue > 1.0f) lineHue -= 1.0f;
            line.color = ImColor::HSV(lineHue, 0.7f, 1.0f);


            for (int i = 0; i < 2; i++) {
                pts[i]->pos.x += pts[i]->vel.x;
                pts[i]->pos.y += pts[i]->vel.y;

                if (pts[i]->pos.x < 0 || pts[i]->pos.x > winSize.x) pts[i]->vel.x *= -1.0f;
                if (pts[i]->pos.y < 0 || pts[i]->pos.y > winSize.y) pts[i]->vel.y *= -1.0f;
            }

            drawList->AddLine(
                ImVec2(winPos.x + line.p1.pos.x, winPos.y + line.p1.pos.y),
                              ImVec2(winPos.x + line.p2.pos.x, winPos.y + line.p2.pos.y),
                              line.color, 2.0f
            );
        }
    }

};//namespace
