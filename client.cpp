#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <vector>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

constexpr int WINDOW_WIDTH = 960;
constexpr int WINDOW_HEIGHT = 540;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  SDL_SetAppMetadata("Game Access Multiplayer Game", "1.0", "multiplayer-game");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer("multiplayer-game", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }
  return SDL_APP_CONTINUE;
}

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

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
}

/*
#include <iostream>
#include <asio.hpp>
using asio::ip::tcp;

asio::awaitable<void> connect(tcp::socket& socket,
    const tcp::resolver::results_type& endpoints) {
  auto [error_code, endpoint] = co_await asio::async_connect(
    socket, endpoints, asio::as_tuple);
  if (error_code) {
    std::cout << "Could not connect: " << error_code.message() << "\n";
    co_return;
  }
  std::cout << "Connected to " << socket.remote_endpoint() << ".\n";
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Usage: client.exe address port\n";
    return 0;
  }
  asio::io_context io_context;
  tcp::socket socket(io_context);
  tcp::resolver resolver(io_context);
  auto endpoints = resolver.resolve(argv[1], argv[2]);
  co_spawn(io_context, connect(socket, endpoints), asio::detached);
  io_context.run();
  return 0;
}
*/