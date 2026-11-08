#include "proxy/socket.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cerrno>
#include <cstddef>
#include <expected>
#include <memory>
#include <span>
#include <system_error>

#include "proxy/file_descriptor.hpp"

namespace proxy {

namespace {
using AddrInfoList = std::unique_ptr<addrinfo, decltype(&freeaddrinfo)>;

class GaiCategory : public std::error_category {
   public:
    const char* name() const noexcept override { return "getaddrinfo"; }
    std::string message(int code) const override { return gai_strerror(code); }
};

const std::error_category& gai_category() {
    static GaiCategory instance;
    return instance;
}

std::expected<AddrInfoList, std::error_code> resolve(std::string_view host, std::uint16_t port) {
    addrinfo hints{};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    std::string host_str{host};
    std::string port_str = std::to_string(port);
    const char* host_ptr = host.empty() ? nullptr : host_str.c_str();
    addrinfo* raw = nullptr;
    int rc = getaddrinfo(host_ptr, port_str.c_str(), &hints, &raw);

    if (rc == EAI_SYSTEM) {
        return std::unexpected(std::error_code{errno, std::system_category()});
    }

    if (rc != 0) {
        return std::unexpected(std::error_code{rc, gai_category()});
    }

    return AddrInfoList{raw, &freeaddrinfo};
}

}  // namespace

Socket::Socket(FileDescriptor fd) noexcept : fd_{std::move(fd)} {}

int Socket::fd() const noexcept { return this->fd_.get(); }

std::expected<Socket, std::error_code> Socket::accept() const {
    FileDescriptor client_fd{::accept(fd(), nullptr, nullptr)};
    if (!client_fd.valid()) {
        return std::unexpected(std::error_code{errno, std::system_category()});
    }

    return Socket{std::move(client_fd)};
}

std::expected<std::size_t, std::error_code> Socket::recv(std::span<std::byte> buffer) const {
    if (buffer.empty()) {
        return std::unexpected(std::make_error_code(std::errc::invalid_argument));
    }
    ssize_t received = ::recv(fd(), buffer.data(), buffer.size(), 0);
    if (received == -1) {
        return std::unexpected(std::error_code{errno, std::system_category()});
    }
    return static_cast<std::size_t>(received);
}

std::expected<std::size_t, std::error_code> Socket::send(std::span<const std::byte> buffer) const {
    if (buffer.empty()) {
        return std::unexpected(std::make_error_code(std::errc::invalid_argument));
    }
    ssize_t sent = ::send(fd(), buffer.data(), buffer.size(), 0);
    if (sent == -1) {
        return std::unexpected(std::error_code{errno, std::system_category()});
    }
    return static_cast<std::size_t>(sent);
}

std::expected<Socket, std::error_code> Socket::listen(std::string_view host, std::uint16_t port) {
    auto addresses = resolve(host, port);
    if (!addresses) {
        return std::unexpected(addresses.error());
    }

    std::error_code last_error = std::make_error_code(std::errc::address_not_available);

    for (addrinfo* ai = addresses->get(); ai != nullptr; ai = ai->ai_next) {
        FileDescriptor fd{::socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)};
        if (!fd.valid()) {
            last_error = std::error_code{errno, std::system_category()};
            continue;
        }

        int yes = 1;
        if (::setsockopt(fd.get(), SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
            last_error = std::error_code{errno, std::system_category()};
            continue;
        }
        if (::bind(fd.get(), ai->ai_addr, ai->ai_addrlen) == -1) {
            last_error = std::error_code{errno, std::system_category()};
            continue;
        }
        if (::listen(fd.get(), SOMAXCONN) == -1) {
            last_error = std::error_code{errno, std::system_category()};
            continue;
        }

        return Socket{std::move(fd)};
    }

    return std::unexpected(last_error);
}

}  // namespace proxy
