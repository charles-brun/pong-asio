#pragma once

#include <cstdint>

namespace pong {
    constexpr int WINDOW_WIDTH = 640;
    constexpr int WINDOW_HEIGHT = 480;

    constexpr float PADDLE_HEIGHT_RATIO = 1.f / 6.f;
    constexpr float PADDLE_WIDTH_RATIO = 1.f / 100.f;

    constexpr float BALL_DIAMETER_RATIO = 2.f / 100.f;

    constexpr float BALL_LAUNCH_COOLDOWN = 2.f;
    constexpr float PADDLE_SPEED_RATIO = 1.f;
    constexpr float BALL_SPEED_RATIO = 1.f / 2.f;

    constexpr uint32_t WIN_SCORE = 5;

    constexpr uint16_t PORT = 40000;

    constexpr double SERVER_TICK = 1.f / 120.f;

    constexpr float PADDLE_WIDTH = WINDOW_WIDTH * PADDLE_WIDTH_RATIO;
    constexpr float PADDLE_HEIGHT = WINDOW_HEIGHT * PADDLE_HEIGHT_RATIO;
    constexpr float BALL_RADIUS = WINDOW_WIDTH * BALL_DIAMETER_RATIO * 0.5f;
    constexpr float PADDLE_SPEED = WINDOW_HEIGHT * PADDLE_SPEED_RATIO;
    constexpr float BALL_SPEED = WINDOW_WIDTH * BALL_SPEED_RATIO;

    struct Vec2 {
        float x;
        float y;
    };

    struct Paddle {
        float x;
        float y;
    };

    struct Ball {
        Vec2 position;
        Vec2 velocity;
    };

    enum class MatchState : uint8_t {
        WaitingPlayer,
        WaitingStart,
        Running,
        GameOver
    };

    struct GameState {
        Paddle left;
        Paddle right;

        Ball ball;

        uint32_t scoreLeft = 0;
        uint32_t scoreRight = 0;

        bool player1Ready;
        bool player2Ready;

        float ballLaunchTimer;

        MatchState matchState = MatchState::WaitingPlayer;

        uint8_t winner;
    };
}
