#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

#include "network.hpp"

namespace pong {
    class Renderer {
    public:
        Renderer(SDL_Renderer* renderer);
        ~Renderer();

        void render(const StateMessage& state);
        void drawPaddle(float x, float y);
        void drawBall(float x, float y);
        void drawCenteredText(const std::string& text, int y);
        void drawText(const char* text, int x, int y);

    private:
        SDL_Renderer* renderer;
        TTF_Font* font;
    };
}
