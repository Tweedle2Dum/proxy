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
    // 1. Ask the kernel for a TCP socket. We get back a file descriptor.
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        std::println(stderr, "socket: {}", std::strerror(errno));
        return 1;
    }
    std::println("got file descriptor {}", listen_fd);

    // 2. Allow reusing the port right after a restart.
    int yes = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        std::println(stderr, "setsockopt: {}", std::strerror(errno));
        return 1;
    }

    // 3. Describe the address we want: any local IP, port 9000.
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9000);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 4. Attach the socket to that address.
    if (bind(listen_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1) {
        std::println(stderr, "bind: {}", std::strerror(errno));
        return 1;
    }

    // 5. Mark it as a listening socket.
    if (listen(listen_fd, SOMAXCONN) == -1) {
        std::println(stderr, "listen: {}", std::strerror(errno));
        return 1;
    }

    std::println("listening on port 9000, press Enter to quit");
    while(true){
        // 1. Wait for a client. accept() blocks until one is in the backlog.
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(listen_fd, reinterpret_cast<sockaddr*>(&client_addr), &client_len);
        if (client_fd == -1) {
                   std::println(stderr, "accept: {}", std::strerror(errno));
                   continue;
        }

        // 2. Print who connected.
        std::array<char, INET_ADDRSTRLEN> ip{};
        inet_ntop(AF_INET, &client_addr.sin_addr, ip.data(), ip.size());
        std::println("client {}:{} connected, fd {}", ip.data(), ntohs(client_addr.sin_port), client_fd);

        // 3. Echo loop: read some bytes, send them back, repeat.
        std::array<std::byte, 1024> buf{};

        while(true){
            ssize_t received = recv(client_fd, buf.data(), buf.size(), 0);
            if (received == 0) {
                std::println("client disconnected");
                break;
            }
            if(received == -1){
                std::println(stderr, "recv: {}", std::strerror(errno));
                break;
            }

            std::println("received {} bytes", received);

            ssize_t sent = send(client_fd, buf.data(), static_cast<std::size_t>(received), 0);
            if (sent == -1) {
                std::println(stderr, "send: {}", std::strerror(errno));
                break;
            }

        }

        close(client_fd);

    }
}
