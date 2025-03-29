#include <iostream>
#include <asio.hpp>
using asio::ip::tcp;

asio::awaitable<void> communicate(tcp::socket socket) {
  std::cout << "There is a new connection from "<< socket.remote_endpoint() << "!\n";
  co_return;
}

asio::awaitable<void> listener(asio::ip::port_type port) {
  auto executor = co_await asio::this_coro::executor;
  std::cout << "Staring a server on port " << port << "...\n";
  tcp::acceptor acceptor(executor, {tcp::v4(), port});
  while (true) {
    auto [ec, socket] = co_await acceptor.async_accept(asio::as_tuple);
    if (ec) {
      std::cout << "Could not accept a new connection: " << ec.message() << "\n";
      co_return;
    }
    asio::co_spawn(executor, communicate(std::move(socket)), asio::detached);
  }
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: server.exe port\n";
    return 0;
  }

  asio::io_context io_context;
  asio::signal_set signals(io_context, SIGINT, SIGTERM);
  signals.async_wait([&](asio::error_code _ec, int _signal){ io_context.stop(); });

  asio::ip::port_type port;
  std::from_chars(argv[1], argv[1] + std::strlen(argv[1]), port);
  co_spawn(io_context, listener(port), asio::detached);

  io_context.run();
  return 0;
}