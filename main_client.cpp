#include <boost/asio.hpp>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>

#include <iostream>

#include "client.h"
#include "include/renderer.h"
#include "include/utils.h"

namespace asio = boost::asio;

int main(int argc, char** argv) {

    FreeConsole();
    std::string host = "127.0.0.1";
    if(argc > 1)
        host = argv[1];

    asio::io_context io;
    pong::Client client(io, host, pong::PORT);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL init failed\n";
        exit(1);
    }

    if (TTF_Init() != 0) {
        std::cerr << "SDL_ttf init failed\n";
        exit(1);
    }

    SDL_Window* window = SDL_CreateWindow(
        "Pong",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        pong::WINDOW_WIDTH,
        pong::WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    pong::Renderer gameRenderer(renderer);

    bool running = true;

    while (running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    running = false;
                    break;

                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_UP) {
                        client.sendInput(pong::InputType::UpPressed);
                    }
                    if (event.key.keysym.sym == SDLK_DOWN) {
                        client.sendInput(pong::InputType::DownPressed);
                    }
                    break;

                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_UP) {
                        client.sendInput(pong::InputType::UpReleased);
                    }
                    if (event.key.keysym.sym == SDLK_DOWN) {
                        client.sendInput(pong::InputType::DownReleased);
                    }
                    if (event.key.keysym.sym == SDLK_SPACE) {
                        client.sendRestart();
                    }
                    break;
            }
        }

        if (!client.isConnected()) {
            exit(0);
        }

        pong::StateMessage state = client.getState();
        gameRenderer.render(state);

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();

    exit(0);
}
