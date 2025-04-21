#pragma once

#include <SDL3/SDL.h>

class Renderer {
  static constexpr int WINDOW_WIDTH = 960;
  static constexpr int WINDOW_HEIGHT = 736;
  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;
  SDL_Texture *map_texture = nullptr;
  SDL_Texture *char_textures[6];

  SDL_AppResult load_texture(SDL_Texture** texture, const char* filename);

public:
  void update();
  SDL_AppResult init();
  void quit();
};

extern Renderer renderer;