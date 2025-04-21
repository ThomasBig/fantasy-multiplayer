#pragma once

#include <SDL3/SDL.h>

#include "player.hpp"
#include "players.hpp"

class Game {
  Players players;
  Player player;
  int player_id = 0;
  Uint64 last_update = 0;
  bool holding_left = false;
  bool holding_right = false;
  bool holding_up = false;
  bool holding_down = false;

public:
  int get_player_id();
  Players const& get_players();
  Player const& get_player();
  void update_state_locally();
  void update_state_from_net(int player_id, std::string serialized);
  void key_press(SDL_Scancode scancode);
  void key_release(SDL_Scancode scancode);
};

extern Game game;
