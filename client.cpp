// standard libraries
#include <iostream>
// rendering using sdl3
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
// networking using asio
#include <asio.hpp>
#include <asio/experimental/awaitable_operators.hpp>
using namespace asio::experimental::awaitable_operators;
using asio::ip::tcp;
// shared lib
#include "shared.hpp"

class Game {
  Players players;
  Player player;
  int player_id = 0;
  Uint64 last_update = 0;
  int dir_x = 0;
  int dir_y = 0;

public:
  Uint64 get_player_id() {
    return player_id;
  }

  Players const& get_players() {
    return players;
  }

  Player const& get_player() {
    return player;
  }

  void update() {
    Uint64 current_update = SDL_GetTicks();
    Uint64 delta_update = current_update - last_update;

    player.current_x += delta_update * Player::speed * dir_x;
    player.current_y += delta_update * Player::speed * dir_y;
    player.target_x = player.current_x;
    player.target_y = player.current_y;

    for (auto& [_, player] : players.data) {
      player.update(int(delta_update));
    }

    last_update = current_update;
  }

  void update_players(
    int player_id, std::string deserialized) {
    this->player_id = player_id;
    players.deserialize(deserialized);
  }

  void key_press(SDL_Scancode scancode) {
    switch (scancode) {
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
      case SDL_SCANCODE_C:
        player.skin = (player.skin + 1) % 6;
        break;
    }
  }

  void key_release(SDL_Scancode scancode) {
    dir_x = 0;
    dir_y = 0;
  }
} game;

class Network {
  asio::io_context context;
  tcp::socket client_socket;
  bool should_exit = false;

  asio::awaitable<void> writer() {
    while (true) {
      std::string serialized = game.get_player().serialize();
      auto [ec, _count] = co_await client_socket.async_send(
        asio::buffer(serialized.data(), serialized.length()), asio::as_tuple);
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
      auto [ec, len] = co_await client_socket.async_receive(
        asio::buffer(data, 1024), asio::as_tuple);
      std::string received(data, len);
      int n = received.find_first_of(' ');
      game.update_players(
        atoi(received.substr(0, n).c_str()),
        received.substr(n+1));
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

public:
  Network() : client_socket(context) {}

  void start(const char* server_address, const char* server_port) {
    tcp::resolver resolver(context);
    auto endpoints = resolver.resolve(server_address, server_port);
    asio::co_spawn(context, connect(std::move(endpoints)), asio::detached);
  }

  SDL_AppResult update() {
    if (should_exit) {
      return SDL_APP_SUCCESS;
    }
    context.poll();
    return SDL_APP_CONTINUE;
  }
} network;

class Renderer {
  static constexpr int WINDOW_WIDTH = 960;
  static constexpr int WINDOW_HEIGHT = 736;
  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;
  SDL_Texture *map_texture = nullptr;
  SDL_Texture *char_textures[6];

  SDL_AppResult load_texture(SDL_Texture** texture, const char* filename){
    SDL_Surface *surface = NULL;
    char *path = NULL;
    SDL_asprintf(&path, "%s/sprites/%s.bmp", SDL_GetBasePath(), filename);
    surface = SDL_LoadBMP(path);
    if (!surface) {
      SDL_Log("Couldn't load image: %s", SDL_GetError());
      return SDL_APP_FAILURE;
    }
    SDL_free(path);
    *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
      SDL_Log("Couldn't create static texture: %s", SDL_GetError());
      return SDL_APP_FAILURE;
    }
    SDL_SetTextureScaleMode(*texture, SDL_SCALEMODE_NEAREST);
    SDL_DestroySurface(surface);
    return SDL_APP_CONTINUE;
  }

public:
  void update() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    SDL_FRect dst_rect {0, 0, float(WINDOW_WIDTH), float(WINDOW_HEIGHT)};
    SDL_RenderTexture(renderer, map_texture, NULL, &dst_rect);

    for (const auto& [id, player] : game.get_players().data) {
      if (id != game.get_player_id()) {
        SDL_FRect dst_rect {player.current_x, player.current_y, 32.0f, 32.0f};
        int skin = std::max(std::min(player.skin, 5), 0); // clamp 0 to 5
        SDL_RenderTexture(renderer, char_textures[skin], NULL, &dst_rect);
      }
    }

    const Player& player = game.get_player();
    SDL_FRect player_rect{player.current_x, player.current_y, 32.0f, 32.0f};
    SDL_RenderTexture(renderer, char_textures[player.skin], NULL, &player_rect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderRect(renderer, &player_rect);

    SDL_RenderPresent(renderer);
  }

  SDL_AppResult init() {
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

    return SDL_APP_CONTINUE;
  }

  void quit() {
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
} renderer;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  if (argc < 3) {
    std::cout << "Usage: client.exe address port\n";
    return SDL_APP_FAILURE;
  }

  if (SDL_AppResult result = renderer.init(); result != SDL_APP_CONTINUE) {
    return result;
  }

  network.start(argv[1], argv[2]);
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }

  if (event->type == SDL_EVENT_KEY_DOWN) {
    game.key_press(event->key.scancode);
  }
  else if (event->type == SDL_EVENT_KEY_UP) {
    game.key_release(event->key.scancode);
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  game.update();
  renderer.update();
  if (SDL_AppResult update = network.update(); update != SDL_APP_CONTINUE) {
    return update;
  }
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  renderer.quit();
}
