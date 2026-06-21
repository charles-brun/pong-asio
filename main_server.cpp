#include <boost/asio.hpp>
#include <chrono>
#include <thread>
#include <iostream>

#include "include/server.h"
#include "include/utils.h"

int main() {

    try {
        boost::asio::io_context io;
        pong::Server server(io);
        auto next = std::chrono::steady_clock::now();

        while(true) {
            io.poll();
            server.update(pong::SERVER_TICK);
            next += std::chrono::microseconds(static_cast<int>(pong::SERVER_TICK * 1000000));
            std::this_thread::sleep_until(next);
        }
    } catch(const std::exception& error) {
        std::cerr << "Server error: " << error.what() << std::endl;
    }

    exit(0);
}
