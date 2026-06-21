#include "../include/client.h"
#include "../include/network.hpp"

#include <iostream>

namespace pong
{
    Client::Client(boost::asio::io_context& io, const std::string& host, uint16_t port): io(io), socket(io) {
        boost::system::error_code error;
        tcp::resolver resolver(io);

        auto endpoints = resolver.resolve(host, std::to_string(port), error);

        if (error) {
            std::cerr << "Resolve failed: " << error.message() << '\n';
            return;
        }

        boost::asio::connect(socket, endpoints, error);

        if (error) {
            std::cerr << "Connect failed: " << error.message() << '\n';
            return;
        }

        AssignPlayerMessage assign;
        boost::asio::read(socket, boost::asio::buffer(&assign, sizeof(assign)), error);

        if (error) {
            std::cerr << "Handshake failed: " << error.message() << '\n';
            socket.close();
            return;
        }

        playerId = assign.playerId;
        connected = true;

        thread = std::thread(&Client::receiveLoop, this);
    }

    Client::~Client() {
        running = false;
        boost::system::error_code error;

        socket.shutdown(tcp::socket::shutdown_both, error);
        socket.close(error);

        if (thread.joinable()) {
            thread.join();
        }
    }


    void Client::sendInput(InputType input) {
        InputMessage msg;
        msg.input = input;
        boost::system::error_code error;

        boost::asio::write(socket, boost::asio::buffer(&msg, sizeof(msg)), error);

        if (error) {
            connected = false;
        }
    }

    void Client::sendRestart() {
        RestartMessage msg;
        boost::system::error_code error;

        boost::asio::write(socket, boost::asio::buffer(&msg, sizeof(msg)), error);

        if (error) {
            connected = false;
        }
    }

    void Client::receiveLoop() {
        while (running) {
            StateMessage msg;
            boost::system::error_code error;

            boost::asio::read(socket, boost::asio::buffer(&msg, sizeof(msg)), error);

            if (error) {
                connected = false;
                break;
            }

            std::lock_guard<std::mutex> lock(mutex);
            state = msg;
        }
    }

    bool Client::isConnected() const {
        return connected;
    }
}
