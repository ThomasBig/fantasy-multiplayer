#include <iostream>

#include <asio.hpp>
#include <asio/experimental/awaitable_operators.hpp>
using namespace asio::experimental::awaitable_operators;
using asio::ip::tcp;

#include "server.hpp"
Server server;

asio::awaitable<void> Server::writer(int player_id) {
  while (socket != nullptr) {
    std::string serialized = std::to_string(player_id) + " " + players.serialize();
    auto [ec, _count] = co_await sockets.at(player_id).async_send(asio::buffer(serialized.data(), serialized.length()), asio::as_tuple);
    if (ec) {
      std::cout << "Could not send: " << ec.message() << "\n";
      players.data.erase(player_id);
      co_return;
    }
    std::cout << "Wrote to player " << player_id << ": " << serialized << "\n";
    asio::steady_timer timer(co_await asio::this_coro::executor, asio::chrono::seconds(1));
    co_await timer.async_wait(asio::use_awaitable);
  }
}

asio::awaitable<void> Server::reader(int player_id) {
  char data[1024];
  while (true) {
    auto [ec, len] = co_await sockets.at(player_id).async_receive(asio::buffer(data, 1024), asio::as_tuple);
    if (ec) {
      std::cout << "Cannot receive from player " << player_id << ": " << ec.message() << "\n";
      players.data.erase(player_id);
      co_return;
    }
    players.data[player_id].deserialize(std::string(data, len));
  }
}

asio::awaitable<void> Server::connect(tcp::socket socket) {
  std::cout << "There is a new connection from "<< socket.remote_endpoint() << "!\n";

  int player_id = last_id++;
  players.data[player_id] = Player();
  sockets.emplace(player_id, std::move(socket));

  co_await (
    writer(player_id) || reader(player_id)
  );
}

asio::awaitable<void> Server::listener(asio::ip::port_type port) {
  auto executor = co_await asio::this_coro::executor;
  std::cout << "Staring a server on port " << port << "...\n";
  tcp::acceptor acceptor(executor, {tcp::v4(), port});
  while (true) {
    auto [ec, socket] = co_await acceptor.async_accept(asio::as_tuple);
    if (ec) {
      std::cout << "Could not accept a new connection: " << ec.message() << "\n";
      co_return;
    }
    asio::co_spawn(executor, connect(std::move(socket)), asio::detached);
  }
}

asio::awaitable<void> Server::game_loop() {
  int delta_ms = 100;
  while (true) {
    for (auto& [id, player] : players.data) {
      player.update(delta_ms);
    }
    asio::steady_timer timer(co_await asio::this_coro::executor, asio::chrono::milliseconds(delta_ms));
    co_await timer.async_wait(asio::use_awaitable);
  }
}
