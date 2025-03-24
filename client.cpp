#include <iostream>
#include <asio.hpp>
using asio::ip::tcp;

asio::awaitable<void> connect(tcp::socket& socket,
    const tcp::resolver::results_type& endpoints) {
  auto [error_code, endpoint] = co_await asio::async_connect(
    socket, endpoints, asio::as_tuple);
  if (error_code) {
    std::cout << "Could not connect: " << error_code.message() << "\n";
    co_return;
  }
  std::cout << "Connected to " << socket.remote_endpoint() << ".\n";
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Usage: client.exe address port\n";
    return 0;
  }
  asio::io_context io_context;
  tcp::socket socket(io_context);
  tcp::resolver resolver(io_context);
  auto endpoints = resolver.resolve(argv[1], argv[2]);
  co_spawn(io_context, connect(socket, endpoints), asio::detached);
  io_context.run();
  return 0;
}