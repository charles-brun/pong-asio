#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>

namespace pong {
    enum class MessageType : uint8_t {
        AssignPlayer = 1,
        Input,
        State,
        Restart
    };

    enum class InputType : uint8_t {
        UpPressed,
        UpReleased,
        DownPressed,
        DownReleased
    };

    struct PacketHeader {
        MessageType type;
    };

    struct AssignPlayerMessage {
        PacketHeader header = { MessageType::AssignPlayer };
        uint8_t playerId;
    };

    struct InputMessage {
        PacketHeader header = { MessageType::Input };
        InputType input;
    };

    struct RestartMessage {
        PacketHeader header = { MessageType::Restart };
    };

    struct StateMessage {
        PacketHeader header = { MessageType::State };

        float leftY;
        float rightY;

        float ballX;
        float ballY;

        uint32_t scoreLeft = 0;
        uint32_t scoreRight = 0;

        uint8_t matchState;
        uint8_t winner = 0;

        uint8_t playerId;

        uint8_t selfReady;
        uint8_t otherReady;
    };

    template <typename T>
    void writeBytes(char* dst, const T& value) {
        std::memcpy(dst, &value, sizeof(T));
    }

    template <typename T>
    void readBytes(const char* src, T& value) {
        std::memcpy(&value, src, sizeof(T));
    }
}
