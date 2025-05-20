#include <iostream>

#include <asio.hpp>
#include <asio/experimental/awaitable_operators.hpp>
using namespace asio::experimental::awaitable_operators;
using asio::ip::tcp;

#include "network.hpp"

asio::awaitable<void> Network::write_to_server() {
  while (true) {
    std::string serialized = serialize();
    auto [ec, _count] = co_await client_socket.async_send(
      asio::buffer(serialized.data(), serialized.length()), asio::as_tuple);
    if (ec) {
      std::cout << "Could not send the message: " << ec.message() << "\n";
      co_return;
    }
    asio::steady_timer timer(context, asio::chrono::milliseconds(1000/client_update_ticks));
    co_await timer.async_wait(asio::use_awaitable);
  }
}

asio::awaitable<void> Network::read_from_server() {
  char data[1024];
  while (true) {
    auto [ec, len] = co_await client_socket.async_receive(
      asio::buffer(data, 1024), asio::as_tuple);
    if (ec) {
      std::cout << "Could not read the message: " << ec.message() << "\n";
      co_return;
    }
    std::string received(data, len);
    deserialize(received);
  }
}

asio::awaitable<void> Network::connect_to_endpoints(const tcp::resolver::results_type endpoints) {
  auto [ec, _endpoint] = co_await asio::async_connect(
    client_socket, endpoints, asio::as_tuple);
  if (ec) {
    std::cout << "Could not connect: " << ec.message() << "\n";
    co_return;
  }
  co_await (write_to_server() || read_from_server());
  should_exit = true;
}

Network::Network() : client_socket(context) {}

void Network::connect_to_server(const char* server_address, const char* server_port, SerializeFn serialize, DeserializeFn deserialize) {
  this->serialize = serialize;
  this->deserialize = deserialize;
  tcp::resolver resolver(context);
  auto endpoints = resolver.resolve(server_address, server_port);
  asio::co_spawn(context, connect_to_endpoints(std::move(endpoints)), asio::detached);
}

SDL_AppResult Network::receive_updates() {
  if (should_exit) {
    // returns success as we handled all errors manually
    return SDL_APP_SUCCESS;
  }
  context.poll();
  return SDL_APP_CONTINUE;
}
