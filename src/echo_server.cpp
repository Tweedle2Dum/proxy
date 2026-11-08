#include "proxy/file_descriptor.hpp"
#include "proxy/socket.hpp"
#include <arpa/inet.h>
#include <array>
#include <cstddef>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <print>


int main() {

    auto listener = proxy::Socket::listen("localhost", 9000);
    if (!listener.has_value()) {
        std::println(stderr, "socket: {}", listener.error().message());
        return 1;
    }
    std::println("listening on port 9000, fd {}", listener->fd());

    while(true){
        auto client = listener->accept();
        if(!client){
            std::println(stderr, "accept: {}", client.error().message());
            continue;
        }
        std::println("client connected, fd {}", client->fd());
        std::array<std::byte, 1024> buf{};

        while(true){
            auto received = client->recv(buf);
               if (!received) {
                   std::println(stderr, "recv: {}", received.error().message());
                   break;
               }
               if (*received == 0) {
                   std::println("client disconnected");
                   break;
               }

               auto sent = client->send(std::span{buf}.first(*received));
               if (!sent) {
                   std::println(stderr, "send: {}", sent.error().message());
                   break;
               }
        }

    }
}
