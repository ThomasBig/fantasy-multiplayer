#pragma once

#include <asio.hpp>
#include <SDL3/SDL.h>

using SerializeFn = std::function<std::string()>;
using DeserializeFn = std::function<void(std::string)>;

class Network {
  static constexpr int client_update_ticks = 1; // 1 update per second
  asio::io_context context;
  asio::ip::tcp::socket client_socket;
  SerializeFn serialize;
  DeserializeFn deserialize;
  bool should_exit = false;

private:
  asio::awaitable<void> write_to_server();
  asio::awaitable<void> read_from_server();
  asio::awaitable<void> connect_to_endpoints(const asio::ip::tcp::resolver::results_type endpoints);

public:
  Network();
  void connect_to_server(const char* server_address, const char* server_port, SerializeFn serialize, DeserializeFn deserialize);
  SDL_AppResult receive_updates();
};
