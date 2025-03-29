---
marp: true
class: invert
---

# Let’s Make an Multiplayer Game Without an Engine

... WHAT?

---

# Premise

* We will learn how to make a simple multiplayer game without losing our mind
* We will use C++20 coroutines and ASIO library for basic networking objects
* We will NOT make production ready code
* We will MAKE a functional and understandable PROTOTYPE

---

# Speaker
* Tomáš Janoušek
* Grew up on: WoW, Counter-Strike, Overwatch, Minecraft
* Wrote a master's thesis on networking
* Worked on: TopSpin 2K25, Mafia: The Old Country, ArmA Games

---

# Recommended Tools
* VS Code (C++, CMake and CMake Tools Extensions)
* C++20 (Coroutines)
* CMake
* Git

---

# Hello, server!
*First we will show an example, how to connect to a server with asio library
without using coroutines*

---

# Connecting to a Server 1/2

```c++
#include <iostream>
#include <asio.hpp>
using asio::ip::tcp;

void connect(tcp::socket& socket, const tcp::resolver::results_type& endpoints);

int main(int argc, char* argv[]) {
  asio::io_context context;  // executes asynchronous tasks
  tcp::socket socket(context);  // connection between computers
  tcp::resolver resolver(context);  // address finder
  auto endpoints = resolver.resolve("127.0.0.1", "8080");
  connect(socket, endpoints); // connect socket to address, next slide
  context.run(); // run all queued tasks
  return 0;
}
```

---

# Connecting to a Server 2/2

```c++
void connect(tcp::socket& socket, const tcp::resolver::results_type& endpoints) {
  asio::async_connect(socket, endpoints, [](std::error_code ec, tcp::endpoint)
  {
    if (ec)
    {
      std::cout << "Could not connect: " << ec.message() << "\n";
      return;
    }
    std::cout << "Connected.";
  });
}
```
---

# Introspection
* To connect to server, we had to call create a context object, socket
and resolver
* We used a callback to print to screen connected if we were successful
* If we needed to continue making the program, i.e., to ask player for their
name, we would need to add a new function inside the callback or handle the
state in some state machine
* Thankfully, we can use a better way, a coroutine

---
# Coroutines
*Now we are aware of downsides of callbacks, so let's explore a newer approach.*

---

# Coroutines: What are they?
* Better functions™
* They run and return a value, however, they can be paused (suspended) to wait
for time consuming operations and main thread can do things in the meantime (i.e, draw graphics, handle players input, etc.)
* Not specific to networking
* Similar, but more powerful than Javascript's `async`/`await`

---

# ASIO Coroutines
* Created using `asio::co_spawn(executor, fn, handler)`
* Suspended using `co_await`
* `asio::awaitable<Type>` return type in coroutine signature
* Values returned using `co_return`

---


# Hello world!
```c++
#include <iostream>
#include <asio.hpp>

asio::awaitable<int> timer(asio::io_context& context) {
    asio::steady_timer timer(context, asio::chrono::seconds(3));
    std::cout << "Wait for it... ";
    co_await timer.async_wait(asio::use_awaitable);
    std::cout << "Hello world!";
    co_return 0;
}

int main() {
    asio::io_context context;
    asio::co_spawn(context, timer(context), asio::detached);
    context.run();
    return 0;
}
```

---
# Coroutines Tips
* Use `asio::use_tuple` over `asio::use_awaitable`
* Use shared pointers over references, raw pointers
* Use custom completion handler to handle errors
* See 'Making Online Multiplayer Games in C++' for more info

---

# Client-Server Connection
*Now we will make a functional example in which we will connect a client to
server. Server will wait for any number of connections.*

---

# Client Connection 1/2
```c++
#include <iostream>
#include <asio.hpp>
using asio::ip::tcp;

asio::awaitable<void> connect(tcp::socket& socket,
  const tcp::resolver::results_type& endpoints);

int main(int argc, char* argv[]) {
  asio::io_context io_context;
  tcp::socket socket(io_context);
  tcp::resolver resolver(io_context);
  auto endpoints = resolver.resolve("127.0.0.1", "8080");
  co_spawn(io_context, connect(socket, endpoints), asio::detached);
  io_context.run();
  return 0;
}
```

---

# Client Connection 2/2

```c++
asio::awaitable<void> connect(tcp::socket& socket,
  const tcp::resolver::results_type& endpoints)
{
  auto [error_code, endpoint] = co_await asio::async_connect(
    socket, endpoints, asio::as_tuple);

  if (error_code) {
    std::cout << "Could not connect: " << error_code.message() << "\n";
    co_return;
  }

  std::cout << "Connected to " << socket.remote_endpoint() << ".\n";
}
```

---
# Client Connection with Custom Address
```c++
int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Usage: client.exe address port\n";
    return 0;
  }
  //...
  auto endpoints = resolver.resolve(argv[1], argv[2]);
  co_spawn(io_context, connect(socket, endpoints), asio::detached);
  //...
}
```

---
# Server Connection 1/2

```c++
#include <iostream>
#include <asio.hpp>
using asio::ip::tcp;

asio::awaitable<void> listener();

int main(int argc, char* argv[]) {
  asio::io_context context;
  asio::signal_set signals(context, SIGINT /*interrupt*/, SIGTERM /*terminate*/);
  signals.async_wait([&](asio::error_code _ec, int _signal){ context.stop(); });
  co_spawn(context, listener(), asio::detached);
  context.run();
  return 0;
}
```

---
# Server Connection 2/2
```c++
asio::awaitable<void> listener() {
  auto executor = co_await asio::this_coro::executor;
  std::cout << "Staring a server on port 8080...";
  tcp::acceptor acceptor(executor, {tcp::v4(), 8080});
  auto [ec, socket] = co_await acceptor.async_accept(asio::as_tuple);
  while (true) {
    if (ec) {
      std::cout << "Could not accept a new connection: " << ec.message() << "\n";
      co_return;
    }
    std::cout << "There is a new connection from "<< socket.remote_endpoint() << "!\n";
  }
}
```

---
# Server Connection with Custom Port

```C++
asio::awaitable<void> listener(asio::ip::port_type port) {
  //...
  tcp::acceptor acceptor(executor, {tcp::v4(), port});
  //...
}

int main(int argc, char* argv[]) {
  //...
  asio::ip::port_type port;
  std::from_chars(argv[1], argv[1] + std::strlen(argv[1]), port);
  co_spawn(io_context, listener(port), asio::detached);
  //...
  return 0;
}
```
---

# Test, test, test!
*Before making the fully-fledged game, we need to make sure our basic
functionality is working as intended. We will try that we can connect notebook
to PC. PC will use fixed and wired internet, notebook willuse internet from a mobile hotspot. They use internet from different ISP companies, that is
Vodafone hotspot and O2 wired connection.*

---

# Common reason why server does not connect
* ISP (private IP)
* Firewall
* Router (port forwarding)
* Antivirus (firewall)

---

# How to circumfluent IP, firewall and other protections
* Cloud server (AWS, Azure, Google Cloud): 100s€/month
* Public IP from ISP & Custom Server: 10€/month + Server Cost
* Virtual Private Network (Hamachi, Zero Tier, RadminVPN): various free plans

---

# Virtual Private Network
* All users need to download the same program.
* One player sets up a new private network.
* Other players then connect to that network.
* Then, all can communicate freely as they are on the same network, meaning one player can run the server and others join it.

---

# Testing
* Single machine (localhost)
* Two PCs next to each other but on different networks (whatismyip)
* Playing with friends (multiple PCs far away)

---

# Recap
*So far...*

---
# Recap
* C++20 and ASIO
* Coroutines over callbacks
* Client -> server connection
* Tested over the real internet

---

# SDL3 Big Picture

```c++
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <vector>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

constexpr int WINDOW_WIDTH = 960;
constexpr int WINDOW_HEIGHT = 540;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]);
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event);
SDL_AppResult SDL_AppIterate(void *appstate);
void SDL_AppQuit(void *appstate, SDL_AppResult result);
```

---

# SDL3 AppInit
```c++
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  SDL_SetAppMetadata("Multiplayer Game", "1.0", "multiplayer-game");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer("multiplayer-game",
      WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}
```

---
# SDL3 SDL_AppIterate

```c++
SDL_AppResult SDL_AppIterate(void *appstate) {
  const Uint64 now = SDL_GetTicks();

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);

  std::vector<SDL_FRect> players;
  players.emplace_back(100.0f+sinf(float(now)/1000.0f)*100.0f, 200.0f, 32.0f, 32.0f);
  players.emplace_back(500.0f+sinf(float(now)/1000.0f)*100.0f, 400.0f, 32.0f, 32.0f);
  players.emplace_back(300.0f+sinf(float(now)/1000.0f)*100.0f, 100.0f, 32.0f, 32.0f);

  SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRects(renderer, players.data(), players.size());

  SDL_RenderPresent(renderer);
  return SDL_APP_CONTINUE;
}
```

---
# SDL3 KeyEvent, Quit
```c++
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
  // SDL cleans window and renderer by itself
}
```

---

# Joining the two
```c++
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <asio.hpp>
using asio::ip::tcp;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
asio::io_context context;

constexpr int WINDOW_WIDTH = 960;
constexpr int WINDOW_HEIGHT = 540;

asio::awaitable<void> connect(tcp::socket socket,
    const tcp::resolver::results_type endpoints) {
  auto [error_code, endpoint] = co_await asio::async_connect(
    socket, endpoints, asio::as_tuple);
  if (error_code) {
    std::cout << "Could not connect: " << error_code.message() << "\n";
    co_return;
  }
  std::cout << "Connected to " << socket.remote_endpoint() << ".\n";
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  if (argc < 3) {
    std::cout << "Usage: client.exe address port\n";
    return SDL_APP_FAILURE;
  }

  SDL_SetAppMetadata("Multiplayer Game", "1.0", "multiplayer-game");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer("multiplayer-game",
      WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  tcp::socket socket(context);
  tcp::resolver resolver(context);
  auto endpoints = resolver.resolve(argv[1], argv[2]);
  asio::co_spawn(context, connect(std::move(socket), std::move(endpoints)), asio::detached);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }
  return SDL_APP_CONTINUE;
}

void render() {
  const Uint64 now = SDL_GetTicks();

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);

  std::vector<SDL_FRect> players;
  players.emplace_back(100.0f+sinf(float(now)/1000.0f)*100.0f, 200.0f, 32.0f, 32.0f);
  players.emplace_back(500.0f+sinf(float(now)/1000.0f)*100.0f, 400.0f, 32.0f, 32.0f);
  players.emplace_back(300.0f+sinf(float(now)/1000.0f)*100.0f, 100.0f, 32.0f, 32.0f);

  SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRects(renderer, players.data(), players.size());

  SDL_RenderPresent(renderer);
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  render();
  context.poll();
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
  // SDL cleans window and renderer by itself
}
```