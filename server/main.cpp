#include <iostream>
#include <asio.hpp>
#include "server.hpp"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: server.exe port\n";
    return 0;
  }

  asio::io_context context;
  asio::signal_set signals(context, SIGINT, SIGTERM);
  signals.async_wait([&](asio::error_code _ec, int _signal){ context.stop(); });

  asio::ip::port_type port;
  std::from_chars(argv[1], argv[1] + std::strlen(argv[1]), port);
  asio::co_spawn(context, server.start_listening_on(port), asio::detached);
  asio::co_spawn(context, server.update_game_state(), asio::detached);

  context.run();
  return 0;
}