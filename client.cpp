#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <asio.hpp>
#include <asio/experimental/awaitable_operators.hpp>
using namespace asio::experimental::awaitable_operators;

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
bool should_exit = false;

constexpr int WINDOW_WIDTH = 960;
constexpr int WINDOW_HEIGHT = 736;

Players players;
Player player;

SDL_Texture *map_texture = nullptr;
SDL_Texture *char_textures[6];

SDL_AppResult load_texture(SDL_Texture** texture, const char* filename){
  SDL_Surface *surface = NULL;
  char *png_path = NULL;
  /* SDL_Surface is pixel data the CPU can access. SDL_Texture is pixel data the GPU can access.
  Load a .bmp into a surface, move it to a texture from there. */

  SDL_asprintf(&png_path, "%s/sprites/%s.bmp", SDL_GetBasePath(), filename);  /* allocate a string of the full file path */
  surface = SDL_LoadBMP(png_path);
  if (!surface) {
    SDL_Log("Couldn't load image: %s", SDL_GetError());
     return SDL_APP_FAILURE;
  }

  SDL_free(png_path);  /* done with this, the file is loaded. */

  *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!texture) {
    SDL_Log("Couldn't create static texture: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_SetTextureScaleMode(*texture, SDL_SCALEMODE_NEAREST);

  SDL_DestroySurface(surface);  /* done with this, the texture has a copy of the pixels now. */
  return SDL_APP_CONTINUE;
}

asio::awaitable<void> writer() {
  while (true) {
    std::string serialized = player.serialize();
    auto [ec, _count] = co_await client_socket.async_send(asio::buffer(serialized.data(), serialized.length()), asio::as_tuple);
    if (ec) {
      std::cout << "Could not send the message: " << ec.message() << "\n";
      co_return;
    }
    asio::steady_timer timer(context, asio::chrono::seconds(1));
    co_await timer.async_wait(asio::use_awaitable);
  }
}

asio::awaitable<void> reader() {
  char data[1024];
  while (true) {
    auto [ec, len] = co_await client_socket.async_receive(asio::buffer(data, 1024), asio::as_tuple);
    players.deserialize(std::string(data, len));
  }
}

asio::awaitable<void> connect(const tcp::resolver::results_type endpoints) {
  auto [error_code, endpoint] = co_await asio::async_connect(
    client_socket, endpoints, asio::as_tuple);
  if (error_code) {
    std::cout << "Could not connect: " << error_code.message() << "\n";
    co_return;
  }

  co_await (writer() || reader());

  should_exit = true;
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

  if (SDL_AppResult loaded = load_texture(&map_texture, "map"); loaded != SDL_APP_CONTINUE) {
    return loaded;
  }

  for (int i = 1; i <= 6; i++) {
    std::string filename = std::string("char")+std::to_string(i);
    if (SDL_AppResult loaded = load_texture(&char_textures[i-1], filename.c_str()); loaded != SDL_APP_CONTINUE) {
      return loaded;
    }
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

  SDL_FRect dst_rect {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
  SDL_RenderTexture(renderer, map_texture, NULL, &dst_rect);

  for (const auto& [id, player] : players.data) {
    SDL_FRect dst_rect {player.current_x, player.current_y, 32.0f, 32.0f};
    SDL_RenderTexture(renderer, char_textures[2], NULL, &dst_rect);
  }

  SDL_FRect player_rect{player.current_x, player.current_y, 32.0f, 32.0f};
  SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderRect(renderer, &player_rect);

  SDL_RenderPresent(renderer);
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  if (should_exit) {
    return SDL_APP_FAILURE; // or SDL_APP_SUCCESS
  }

  current_update = SDL_GetTicks();
  delta_update = current_update - last_update;

  player.current_x += delta_update * Player::speed * dir_x;
  player.current_y += delta_update * Player::speed * dir_y;
  player.target_x = player.current_x;
  player.target_y = player.current_y;

  for (auto& [_, player] : players.data) {
    player.update(int(delta_update));
  }

  render();
  context.poll();
  last_update = current_update;
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
  if (map_texture != nullptr) {
    SDL_DestroyTexture(map_texture);
  }
  for (int i = 0; i <= 5; i++) {
    if (char_textures[i] != nullptr) {
      SDL_DestroyTexture(char_textures[i]);
    }
  }
  // SDL cleans window and renderer by itself
}
