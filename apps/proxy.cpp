#include <print>
#include <thread>

#include "proxy/forward.hpp"
#include "proxy/socket.hpp"

namespace {
void handle_connection(proxy::Socket client) {
    auto backend = proxy::Socket::connect("localhost", 9000);
    if (!backend) {
        std::println(stderr, "connect: {}", backend.error().message());
        return;
    }

    std::println("connected to backend");

    {
        std::jthread upstream([&] {
            auto result = proxy::pump(client, *backend);
            if (!result) {
                std::println(stderr, "client -> backend: {}", result.error().message());
            }
        });

        std::jthread downstream([&] {
            auto result = proxy::pump(*backend, client);
            if (!result) {
                std::println(stderr, "backend -> client: {}", result.error().message());
            }
        });
    }

    std::println("connection closed");
}
}  // namespace

int main() {
    auto listener = proxy::Socket::listen("0.0.0.0", 8000);
    if (!listener) {
        std::println(stderr, "listen: {}", listener.error().message());
        return 1;
    }
    std::println("proxy on port 8000, forwarding to localhost:9000");

    while (true) {
        auto client = listener->accept();
        if (!client) {
            std::println(stderr, "accept: {}", client.error().message());
            continue;
        }
        std::thread(handle_connection, std::move(*client)).detach();
    }

    return 0;
}
