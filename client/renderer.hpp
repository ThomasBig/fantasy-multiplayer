#pragma once

#include <SDL3/SDL.h>
#include "player.hpp"
#include "players.hpp"

class Renderer {
  static constexpr int window_width = 960;
  static constexpr int window_height = 736;
  static constexpr int avatars = 6;

  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;
  SDL_Texture *map_texture = nullptr;
  SDL_Texture *char_textures[avatars];

private:
  SDL_AppResult load_texture(SDL_Texture** texture, const char* filename);

public:
  int get_avatars_count();
  SDL_AppResult init();
  void update(int player_id, Player const& player, Players const& players);
  void quit();
};
