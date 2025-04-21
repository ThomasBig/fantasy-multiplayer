#pragma once

#include <asio.hpp>
#include <SDL3/SDL.h>

class Network {
  asio::io_context context;
  asio::ip::tcp::socket client_socket;
  bool should_exit = false;

  asio::awaitable<void> writer();
  asio::awaitable<void> reader();
  asio::awaitable<void> connect(const asio::ip::tcp::resolver::results_type endpoints);

public:
  Network();
  void start(const char* server_address, const char* server_port);
  SDL_AppResult update();
};

extern Network network;
