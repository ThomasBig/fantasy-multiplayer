#pragma once

#include <unordered_map>
#include <asio.hpp>
#include "players.hpp"

class Server {
  static constexpr int server_update_ticks = 100;
  Players players;
  int last_used_id = 0;
  std::unordered_map<int, asio::ip::tcp::socket> sockets;

private:
  asio::awaitable<void> write_updates_to(int player_id);
  asio::awaitable<void> read_updates_from(int player_id);
  asio::awaitable<void> connect_new_socket(asio::ip::tcp::socket socket);

public:
  asio::awaitable<void> start_listening_on(asio::ip::port_type port);
  asio::awaitable<void> update_game_state();
};

extern Server server;
