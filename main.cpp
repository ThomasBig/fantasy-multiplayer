#include <iostream>
#include <asio.hpp>

int main() {
    asio::io_context context;
    asio::steady_timer timer(context, asio::chrono::seconds(3));
    std::cout << "Wait for it... ";
    timer.async_wait([](asio::error_code _err){
        std::cout << "Hello, world!\n";
    });
    context.run();
    return 0;
}
