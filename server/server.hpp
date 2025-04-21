#pragma once

#include <unordered_map>
#include <asio.hpp>
#include "players.hpp"

class Server {
  static constexpr int server_update_ticks = 10; // 10 updates per second
  Players players;
  int last_used_id = 0;
  std::unordered_map<int, asio::ip::tcp::socket> sockets;

private:
  asio::awaitable<void> write_to_player(int player_id);
  asio::awaitable<void> read_from_player(int player_id);
  asio::awaitable<void> connect_new_socket(asio::ip::tcp::socket socket);

public:
  asio::awaitable<void> start_listening_on(asio::ip::port_type port);
  asio::awaitable<void> update_game_state();
};
