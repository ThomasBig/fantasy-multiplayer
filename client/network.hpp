#pragma once

#include <asio.hpp>
#include <SDL3/SDL.h>

using DeserializeFn = std::function<void(std::string)>;
using SerializeFn = std::function<std::string()>;

class Network {
  asio::io_context context;
  asio::ip::tcp::socket client_socket;
  DeserializeFn deserialize;
  SerializeFn serialize;
  bool should_exit = false;

private:
  asio::awaitable<void> write_to_server();
  asio::awaitable<void> read_from_server();
  asio::awaitable<void> connect_to_endpoints(const asio::ip::tcp::resolver::results_type endpoints);

public:
  Network(SerializeFn serialize, DeserializeFn deserialize);
  void connect_to_server(const char* server_address, const char* server_port);
  SDL_AppResult receive_updates();
};
