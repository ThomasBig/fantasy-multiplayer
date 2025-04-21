#pragma once

#include <unordered_map>
#include <asio.hpp>
#include "../shared/players.hpp"

class Server {
  Players players;
  int last_id = 0;
  std::unordered_map<int, asio::ip::tcp::socket> sockets;

public:
  asio::awaitable<void> writer(int player_id);

  asio::awaitable<void> reader(int player_id);

  asio::awaitable<void> connect(asio::ip::tcp::socket socket);

  asio::awaitable<void> listener(asio::ip::port_type port);

  asio::awaitable<void> game_loop();
};

extern Server server;
