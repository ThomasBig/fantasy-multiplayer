#include <asio.hpp>
#include <asio/experimental/awaitable_operators.hpp>
using namespace asio::experimental::awaitable_operators;
using asio::ip::tcp;

#include "game.hpp"

int Game::get_player_id() {
  return player_id;
}

Players const& Game::get_players() {
  return players;
}

Player const& Game::get_player() {
  return player;
}

void Game::update_state_locally() {
  Uint64 current_update = SDL_GetTicks();
  Uint64 delta_update = current_update - last_update;

  int dir_x = holding_right - holding_left;
  int dir_y = holding_down - holding_up;

  player.current_x += delta_update * Player::speed * dir_x;
  player.current_y += delta_update * Player::speed * dir_y;
  player.target_x = player.current_x;
  player.target_y = player.current_y;

  for (auto& [_, player] : players.data) {
    player.update_position(int(delta_update));
  }

  last_update = current_update;
}

std::string Game::serialize() {
  return player.serialize();
}

void Game::deserialize(std::string serialized) {
  int n = serialized.find_first_of(' ');
  this->player_id = atoi(serialized.substr(0, n).c_str());
  players.deserialize(serialized.substr(n+1));
}

void Game::key_press(SDL_Scancode scancode, int avatars_count) {
  switch (scancode) {
    case SDL_SCANCODE_W:
      holding_up = true;
      break;
    case SDL_SCANCODE_A:
      holding_left = true;
      break;
    case SDL_SCANCODE_S:
      holding_down = true;
      break;
    case SDL_SCANCODE_D:
      holding_right = true;
      break;
    case SDL_SCANCODE_C:
      player.avatar = (player.avatar + 1) % avatars_count;
      break;
  }
}

void Game::key_release(SDL_Scancode scancode) {
  switch (scancode) {
    case SDL_SCANCODE_W:
      holding_up = false;
      break;
    case SDL_SCANCODE_A:
      holding_left = false;
      break;
    case SDL_SCANCODE_S:
      holding_down = false;
      break;
    case SDL_SCANCODE_D:
      holding_right = false;
      break;
  }
}
