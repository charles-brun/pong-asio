#include "../include/renderer.h"
#include "../include/utils.h"

#include <string>
#include <iostream>

namespace pong {
    Renderer::Renderer(SDL_Renderer* renderer): renderer(renderer) {
        if (TTF_Init() == 0) {
            font = TTF_OpenFont("../assets/arial.ttf", 24);
        }
    }

    Renderer::~Renderer() {
        if (font) {
            TTF_CloseFont(font);
        }

        TTF_Quit();
    }

    void Renderer::drawPaddle(float x, float y) {
        SDL_Rect r;
        r.x = static_cast<int>(x);
        r.y = static_cast<int>(y);
        r.w = static_cast<int>(PADDLE_WIDTH);
        r.h = static_cast<int>(PADDLE_HEIGHT);

        SDL_RenderFillRect(renderer, &r);
    }

    void Renderer::drawBall(float x, float y) {
        SDL_Rect r;
        r.x = static_cast<int>(x - BALL_RADIUS);
        r.y = static_cast<int>(y - BALL_RADIUS);
        r.w = static_cast<int>(BALL_RADIUS * 2);
        r.h = static_cast<int>(BALL_RADIUS * 2);

        SDL_RenderFillRect(renderer, &r);
    }

    void Renderer::drawCenteredText(const std::string &text, int y) {
        if (!font) {
            return;
        }

        SDL_Color color {255, 255, 255, 255};
        SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);

        if (!surface) {
            return;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        if (!texture) {
            SDL_FreeSurface(surface);
            return;
        }

        SDL_Rect dst;

        dst.w = surface->w;
        dst.h = surface->h;
        dst.x = (WINDOW_WIDTH - dst.w) / 2;
        dst.y = y;

        SDL_FreeSurface(surface);
        SDL_RenderCopy(renderer, texture, nullptr, &dst);
        SDL_DestroyTexture(texture);
    }

    void Renderer::drawText(const char* text, int x, int y) {
        if (!font) {
            return;
        }

        SDL_Color white {255,255,255,255};

        SDL_Surface* surface = TTF_RenderText_Solid(font, text, white);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_Rect dst { x, y, surface->w, surface->h };

        SDL_FreeSurface(surface);

        SDL_RenderCopy(renderer, texture, nullptr, &dst);
        SDL_DestroyTexture(texture);
    }

    void Renderer::render(const StateMessage& state) {
        SDL_SetRenderDrawColor(renderer, 0,0,0,255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255,255,255,255);

        drawPaddle(20, state.leftY - PADDLE_HEIGHT / 2);
        drawPaddle(WINDOW_WIDTH - 20 - PADDLE_WIDTH, state.rightY - PADDLE_HEIGHT / 2);

        drawBall(state.ballX, state.ballY);

        auto matchState = static_cast<MatchState>(state.matchState);

        switch(matchState) {
            case MatchState::WaitingPlayer:
                drawCenteredText("Waiting for another player to join...", 60);
                break;

            case MatchState::WaitingStart:
                if (state.selfReady) {
                    drawCenteredText("Waiting for the other player...", 60);
                } else {
                    drawCenteredText("- Press Space to play -", 60);
                }
                break;

            case MatchState::GameOver:
                if (state.playerId == state.winner) {
                    drawCenteredText("You won !", 20);
                } else {
                    drawCenteredText("You lose", 20);
                }

                if (state.selfReady) {
                    drawCenteredText("Wainting for the other player...", 60);
                } else {
                    drawCenteredText("- Press Space to play again -", 60);
                }
                break;

            default:
                break;
        }
        
        drawText(std::to_string(state.scoreLeft).c_str(), WINDOW_WIDTH / 4, 20);
        drawText(std::to_string(state.scoreRight).c_str(), WINDOW_WIDTH * 3 / 4, 20);

        SDL_RenderPresent(renderer);
    }
}
