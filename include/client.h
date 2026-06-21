#pragma once

#include <boost/asio.hpp>
#include <thread>
#include <mutex>
#include <atomic>

#include "network.hpp"

namespace pong {
    using boost::asio::ip::tcp;

    class Client {
    public:
        Client(boost::asio::io_context& io, const std::string& host, uint16_t port);
        ~Client();

        uint8_t getPlayerId() const { return  playerId; }

        StateMessage getState() const {
            std::lock_guard<std::mutex> lock(mutex);
            return state;
        }

        void sendInput(InputType input);
        void sendRestart();

        bool isConnected() const;

    private:
        void receiveLoop();

        bool connected = false;
        boost::asio::io_context& io;
        tcp::socket socket;

        std::thread thread;
        std::atomic<bool> running = true;

        mutable std::mutex mutex;
        StateMessage state;
        uint8_t playerId;
    };
}
