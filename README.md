## Pong Asio
Basic Pong game playable online. The two client instances needs a server for running.

### Requirements
This project uses [Boost.Asio](https://www.boost.org/doc/libs/latest/doc/html/boost_asio.html) for networking, and [SDL2](https://github.com/libsdl-org/SDL/releases/tag/release-2.32.4) + [SDL2_TTF](https://github.com/libsdl-org/SDL_ttf/releases/tag/release-2.24.0) for rendering.

Make sure to install those before building the client and server.

### How to build
Use [CMake](https://cmake.org/download/) (v3.20 minimum) to build the project.

### How to use
The `server.exe` file will run the server on the port `40000` until you close the console window.

Run `client.exe [server host IP]` to launch a client instance connected to the server. Not passing an IP arg results in default IP being localhost (127.0.0.1), if you want to run a client on the same device as the host.

Two clients max can join a server (this is Pong).

### How to play
Press `Space` key to start. When both players are ready, the game starts.

Use `Up` and `Down` arrows to move your paddle and throw the ball back.

The first player getting 5 points wins the game.
