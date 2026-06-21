#include "../include/server.h"
#include "../include/utils.h"

#include <cmath>
#include <cstdlib>

namespace pong {
    Server::Server(boost::asio::io_context& io): io(io), acceptor(io, tcp::endpoint(tcp::v4(), PORT)) {
        state.left.x = 40.f;
        state.right.x = WINDOW_WIDTH - 40.f;

        state.left.y = WINDOW_HEIGHT * 0.5f;
        state.right.y = WINDOW_HEIGHT * 0.5f;

        resetBall(true);
        accept();
    }

    void Server::accept() {
        auto socket = std::make_shared<tcp::socket>(io);

        acceptor.async_accept(*socket, [this, socket](const boost::system::error_code& error) {
                if (error) {
                    return;
                }

                if(clients.size() >= 2) {
                    socket->close();
                    accept();
                    return;
                }

                auto client = std::make_shared<Client>();
                client->socket = socket;
                client->playerId = static_cast<uint8_t>(clients.size() + 1);

                clients.push_back(client);

                AssignPlayerMessage msg;
                msg.playerId = client->playerId;

                boost::asio::write(*socket, boost::asio::buffer(&msg, sizeof(msg)));

                startReceive(client);
                accept();
            });
    }

    void Server::startReceive(std::shared_ptr<Client> client)
    {
        auto header = std::make_shared<PacketHeader>();

        boost::asio::async_read(*client->socket, boost::asio::buffer(header.get(), sizeof(PacketHeader)),
            [this, client, header](const boost::system::error_code& error, std::size_t) {
                if (error) {
                    clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());

                    if (clients.size() < 2) {
                        state.matchState = MatchState::WaitingPlayer;

                        state.player1Ready = false;
                        state.player2Ready = false;
                    }

                    return;
                }

                switch(header->type) {
                    case MessageType::Input: {
                        auto msg = std::make_shared<InputMessage>();

                        boost::asio::async_read(*client->socket, boost::asio::buffer(&msg->input, sizeof(InputType)),
                            [this, client, msg](const boost::system::error_code& inputError, std::size_t) {
                            if (inputError) {
                                clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());

                                if(clients.size() < 2) {
                                    state.matchState = MatchState::WaitingPlayer;

                                    state.player1Ready = false;
                                    state.player2Ready = false;
                                }

                                return;
                            }

                            processInput(client, *msg);
                            startReceive(client);
                        });
                        break;
                    }

                    case MessageType::Restart: {
                        processRestart(client);
                        startReceive(client);
                        break;
                    }

                    default:
                        startReceive(client);
                        break;
                }
            });
    }

    void Server::processInput(std::shared_ptr<Client> client, const InputMessage& msg) {
        switch(msg.input) {
            case InputType::UpPressed:
                client->up = true;
                break;

            case InputType::UpReleased:
                client->up = false;
                break;

            case InputType::DownPressed:
                client->down = true;
                break;

            case InputType::DownReleased:
                client->down = false;
                break;
        }
    }

    void Server::processRestart(std::shared_ptr<Client> client) {
        if (client->playerId == 1) {
            state.player1Ready = true;
        }
        if (client->playerId == 2) {
            state.player2Ready = true;
        }
        if (state.player1Ready && state.player2Ready) {
            restartMatch();
        }
    }

    void Server::restartMatch() {
        state.scoreLeft = 0;
        state.scoreRight = 0;

        state.matchState = MatchState::Running;
        state.winner = 0;

        state.player1Ready = false;
        state.player2Ready = false;

        resetBall(true);
    }

    bool Server::intersects(float x, float y, float radius, const Paddle& paddle) {
        float halfW = PADDLE_WIDTH * 0.5f;
        float halfH = PADDLE_HEIGHT * 0.5f;

        float left = paddle.x - halfW;
        float right = paddle.x + halfW;
        float top = paddle.y - halfH;
        float bottom = paddle.y + halfH;

        return x + radius >= left && x - radius <= right && y + radius >= top && y - radius <= bottom;
    }

    void Server::resetBall(bool toRight) {
        state.ballLaunchTimer = 2.f;
        state.ball.position = { WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.5f };

        float angle = (-45.f + (std::rand() % 90)) * 3.14159f / 180.f;
        float dir = toRight ? 1.f : -1.f;

        state.ball.velocity = { dir * BALL_SPEED * std::cos(angle), BALL_SPEED * std::sin(angle) };
    }

    void Server::updatePaddles(double dt) {
        for(auto& client : clients)
        {
            Paddle* paddle = client->playerId == 1 ? &state.left : &state.right;

            if (client->up) {
                paddle->y -= PADDLE_SPEED * dt;
            }
            if (client->down) {
                paddle->y += PADDLE_SPEED * dt;
            }

            float half = PADDLE_HEIGHT * 0.5f;

            if (paddle->y < half) {
                paddle->y = half;
            }
            if (paddle->y > WINDOW_HEIGHT - half) {
                paddle->y = WINDOW_HEIGHT - half;
            }
        }
    }

    void Server::updateBall(double dt)
    {
        if (state.ballLaunchTimer <= 0) {
            auto& ball = state.ball;

            ball.position.x += ball.velocity.x * dt;
            ball.position.y += ball.velocity.y * dt;

            float radius = BALL_RADIUS;

            if (ball.position.y < radius) {
                ball.position.y = radius;
                ball.velocity.y *= -1;
            }

            if (ball.position.y > WINDOW_HEIGHT - radius) {
                ball.position.y = WINDOW_HEIGHT - radius;
                ball.velocity.y *= -1;
            }

            if (intersects(ball.position.x, ball.position.y, radius, state.left)) {
                ball.velocity.x = std::abs(ball.velocity.x);
            }

            if (intersects(ball.position.x, ball.position.y, radius, state.right)) {
                ball.velocity.x = -std::abs(ball.velocity.x);
            }

            if (ball.position.x < 0) {
                state.scoreRight++;
                resetBall(true);
            }

            if (ball.position.x > WINDOW_WIDTH) {
                state.scoreLeft++;
                resetBall(false);
            }

            if (state.scoreLeft >= WIN_SCORE) {
                state.matchState = MatchState::GameOver;
                state.winner = 1;
            }

            if (state.scoreRight >= WIN_SCORE) {
                state.matchState = MatchState::GameOver;
                state.winner = 2;
            }

            ball.velocity.x *= 1.f + dt * 0.02f;
            ball.velocity.y *= 1.f + dt * 0.02f;
        } else {
            state.ballLaunchTimer -= dt;
        }
    }

    void Server::sendState() {
        StateMessage msg;

        msg.leftY = state.left.y;
        msg.rightY = state.right.y;

        msg.ballX = state.ball.position.x;
        msg.ballY = state.ball.position.y;

        msg.scoreLeft = state.scoreLeft;
        msg.scoreRight = state.scoreRight;

        msg.winner = state.winner;
        msg.matchState = static_cast<uint8_t>(state.matchState);

        std::vector<std::shared_ptr<Client>> disconnected;

        for (auto& client : clients) {
            msg.playerId = client->playerId;

            if (client->playerId == 1) {
                msg.selfReady = state.player1Ready;
                msg.otherReady = state.player2Ready;
            }
            else {
                msg.selfReady = state.player2Ready;
                msg.otherReady = state.player1Ready;
            }

            boost::system::error_code error;
            boost::asio::write(*client->socket, boost::asio::buffer(&msg, sizeof(msg)), error);

            if (error) {
                disconnected.push_back(client);
            }
        }

        for (auto& client: disconnected) {
            clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
        }

        if (clients.size() < 2) {
            state.matchState = MatchState::WaitingPlayer;

            state.player1Ready = false;
            state.player2Ready = false;
        }
    }

    void Server::update(double dt) {
        if (clients.size() < 2) {
            state.matchState = MatchState::WaitingPlayer;
            sendState();
            return;
        }

        switch (state.matchState) {
            case MatchState::WaitingPlayer:
                state.matchState = MatchState::WaitingStart;
                break;

            case MatchState::Running:
                updateBall(dt);
                break;

            default:
                break;
        }

        updatePaddles(dt);
        sendState();
    }
}
