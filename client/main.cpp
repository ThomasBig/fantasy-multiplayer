#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "game.hpp"
#include "network.hpp"
#include "renderer.hpp"

Game game;
Renderer renderer;
Network network(
  []{return game.get_player().serialize();},
  [](std::string serialized){return game.update_state_from_net(serialized);}
);

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  if (argc < 3) {
    SDL_Log("Usage: client.exe address port");
    return SDL_APP_FAILURE;
  }

  if (SDL_AppResult result = renderer.init(); result != SDL_APP_CONTINUE) {
    return result;
  }

  network.connect_to_server(argv[1], argv[2]);
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }

  if (event->type == SDL_EVENT_KEY_DOWN) {
    game.key_press(event->key.scancode, renderer.get_avatars_count());
  }
  else if (event->type == SDL_EVENT_KEY_UP) {
    game.key_release(event->key.scancode);
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  game.update_state_locally();
  renderer.update(game.get_player_id(), game.get_player(), game.get_players());
  if (SDL_AppResult update = network.receive_updates(); update != SDL_APP_CONTINUE) {
    return update;
  }
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  renderer.quit();
}
