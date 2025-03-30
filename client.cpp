#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <asio.hpp>
#include "shared.hpp"
using asio::ip::tcp;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
asio::io_context context;
tcp::socket client_socket(context);
Uint64 last_update = 0;
Uint64 current_update = 0;
Uint64 delta_update;
int dir_x = 0;
int dir_y = 0;

constexpr int WINDOW_WIDTH = 960;
constexpr int WINDOW_HEIGHT = 540;

Players players;
Player player;

asio::awaitable<void> writer() {
  while (true) {
    std::string serialized = player.serialize();
    auto [ec, _count] = co_await client_socket.async_send(asio::buffer(serialized.data(), serialized.length()), asio::as_tuple);
    if (ec) {
      // std::cout << "Could not send the message: " << ec.message() << "\n";
      co_return;
    }
    // std::cout << "Wrote to server " << serialized << "\n";
    asio::steady_timer timer(context, asio::chrono::seconds(1));
    co_await timer.async_wait(asio::use_awaitable);
  }
}

asio::awaitable<void> reader() {
  char data[1024];
  while (true) {
    auto [ec, message] = co_await client_socket.async_receive(asio::buffer(data, 1024), asio::as_tuple);
    // std::cout << "Read from server " << data<< "\n";
    players.update(data);
  }
}

asio::awaitable<void> connect(const tcp::resolver::results_type endpoints) {
  auto [error_code, endpoint] = co_await asio::async_connect(
    client_socket, endpoints, asio::as_tuple);
  if (error_code) {
    std::cout << "Could not connect: " << error_code.message() << "\n";
    co_return;
  }
  // std::cout << "Connected to " << client_socket.remote_endpoint() << ".\n";
  co_spawn(context, writer(), asio::detached);
  co_spawn(context, reader(), asio::detached);
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

  tcp::resolver resolver(context);
  auto endpoints = resolver.resolve(argv[1], argv[2]);
  asio::co_spawn(context, connect(std::move(endpoints)), asio::detached);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }
  dir_x = 0;
  dir_y = 0;
  if (event->type == SDL_EVENT_KEY_DOWN) {
    switch (event->key.scancode) {
      case SDL_SCANCODE_W:
        dir_y = -1;
        break;
      case SDL_SCANCODE_A:
        dir_x = -1;
        break;
      case SDL_SCANCODE_S:
        dir_y = 1;
        break;
      case SDL_SCANCODE_D:
        dir_x = 1;
        break;
    }
  }
  return SDL_APP_CONTINUE;
}

void render() {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);

  std::vector<SDL_FRect> rects;
  for (const auto& [id, player] : players.data) {
    rects.emplace_back(player.current_x, player.current_y, 32.0f, 32.0f);
  }

  SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRects(renderer, rects.data(), rects.size());

  SDL_FRect player_rect{player.current_x, player.current_y, 32.0f, 32.0f};
  SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderRect(renderer, &player_rect);

  SDL_RenderPresent(renderer);
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  current_update = SDL_GetTicks();
  delta_update = current_update - last_update;

  player.current_x += delta_update * Player::speed * dir_x;
  player.current_y += delta_update * Player::speed * dir_y;

  render();
  context.poll();
  last_update = current_update;
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
  // SDL cleans window and renderer by itself
}
