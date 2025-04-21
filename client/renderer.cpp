#include "game.hpp"
#include "renderer.hpp"

Renderer renderer;

SDL_AppResult Renderer::load_texture(SDL_Texture** texture, const char* filename){
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

void Renderer::update() {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);

  SDL_FRect dst_rect {0, 0, float(WINDOW_WIDTH), float(WINDOW_HEIGHT)};
  SDL_RenderTexture(renderer, map_texture, NULL, &dst_rect);

  for (const auto& [id, player] : game.get_players().data) {
    if (id != game.get_player_id()) {
      SDL_FRect dst_rect {player.current_x, player.current_y, 32.0f, 32.0f};
      int avatar = std::max(std::min(player.avatar, 5), 0); // clamp 0 to 5
      SDL_RenderTexture(renderer, char_textures[avatar], NULL, &dst_rect);
    }
  }

  const Player& player = game.get_player();
  SDL_FRect player_rect{player.current_x, player.current_y, 32.0f, 32.0f};
  SDL_RenderTexture(renderer, char_textures[player.avatar], NULL, &player_rect);

  SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderRect(renderer, &player_rect);

  SDL_RenderPresent(renderer);
}

SDL_AppResult Renderer::init() {
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

void Renderer::quit() {
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
