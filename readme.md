# Fantasy Multiplayer
*Simple Online Multiplayer Game, made with ASIO, SDL3 and C++.*

**Author:** Tomáš Janoušek

**Motivation:** Multiplayer games do not have to be made using game engines.

![](game.png)

## Client executable
Game client which renders graphics in a window, handles key presses and handles networking in the background. Also shows a yellow border around
your character.

Usage: `client.exe address port`
* **address** - server address to connect to (such as `127.0.0.1` for localhost or `12.34.56.789` for some IPv4 address)
* **port** - sever port to connect to (such as `5050` or `55555`)

Keyboard keys:
* **W** - move up
* **A** - move left
* **S** - move down
* **D** - move right
* **C** - change avatar

## Server executable
Headless server which updates connected clients with their location and avatar info.

Usage: `server.exe port`
* **port** - port number where to start server (such as `5050` or `55555`)

## Recommended Programs
**Visual Studio 2020 C++ compiler**: to compile C++20 code\
**Git**: to download ASIO library and versioning\
**VS Code**: to write code\
*C/C++ Extension*: IntelliSense, Debugging\
*CMake Extension*: Syntax for CMake files\
*CMake Tools Extension*: Buttons for easier compilation

## Installation
1. Install recommended programs
2. Clone this repository
3. Clone **ASIO 1.32.0** in the repository folder: `git clone --depth 1 --branch asio-1-32-0 https://github.com/chriskohlhoff/asio.git`
4. Clone **SDL 3.2** in the repository folder: `git clone --depth 1 --branch release-3.2.x https://github.com/libsdl-org/SDL.git vendored/SDL`


## License
```
Code:
    Fantasy Multiplayer
    Author: Tomas Janousek
    Creation Date: 06-04-2025
    License: MIT License

Sprites:
    Tiny Dungeon (1.0)
    Author: Kenney (www.kenney.nl)
    Creation date: 05-07-2022
    License: (Creative Commons Zero, CC0)

    Adventure Awaits Asset Pack (1.0)
    Author: Ishtar Pixels
    Creation date: 20-10-2023
    License: (Creative Commons Zero, CC0)
```