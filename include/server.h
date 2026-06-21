#pragma once

#include <boost/asio.hpp>

#include <memory>
#include <vector>

#include "utils.h"
#include "network.hpp"

namespace pong {
    using boost::asio::ip::tcp;

    class Server {
    public:
        Server(boost::asio::io_context& io);

        void update(double dt);

    private:
        struct Client {
            std::shared_ptr<tcp::socket> socket;
            uint8_t playerId;
            bool up;
            bool down;
            bool restart;
        };

        void accept();
        void startReceive(std::shared_ptr<Client> client);

        void processInput(std::shared_ptr<Client> client, const InputMessage& msg);
        void processRestart(std::shared_ptr<Client> client);

        void updatePaddles(double dt);
        void updateBall(double dt);

        void resetBall(bool toRight);
        void restartMatch();

        void sendState();

        static bool intersects(float x, float y, float radius, const Paddle& paddle);

        boost::asio::io_context& io;
        tcp::acceptor acceptor;

        std::vector<std::shared_ptr<Client>> clients;

        GameState state;
    };
}
