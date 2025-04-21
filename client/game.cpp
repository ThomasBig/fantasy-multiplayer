#include <iostream>

#include <SDL3/SDL.h>

#include "player.hpp"
#include "players.hpp"
#include "game.hpp"

#include <asio.hpp>
#include <asio/experimental/awaitable_operators.hpp>
using namespace asio::experimental::awaitable_operators;
using asio::ip::tcp;

Game game;

Uint64 Game::get_player_id() {
  return player_id;
}

Players const& Game::get_players() {
  return players;
}

Player const& Game::get_player() {
  return player;
}

void Game::update() {
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

void Game::update_players(
  int player_id, std::string deserialized) {
  this->player_id = player_id;
  players.deserialize(deserialized);
}

void Game::key_press(SDL_Scancode scancode) {
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
      player.avatar = (player.avatar + 1) % 6;
      break;
  }
}

void Game::key_release(SDL_Scancode scancode) {
  dir_x = 0;
  dir_y = 0;
}
