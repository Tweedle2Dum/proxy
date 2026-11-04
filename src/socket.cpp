#include "proxy/socket.hpp"
#include "proxy/file_descriptor.hpp"
#include <arpa/inet.h>
#include <cstddef>
#include <span>
#include <sys/socket.h>
#include <cerrno>
#include <expected>
#include <netinet/in.h>
#include <print>
#include <sys/socket.h>
#include <sys/types.h>
#include <system_error>
namespace proxy {

    Socket::Socket(FileDescriptor fd) noexcept : fd_{std::move(fd)} {}

    int Socket::fd() const noexcept {
        return this->fd_.get();
    }

    std::expected<Socket, std::error_code>Socket::accept() const {
        FileDescriptor client_fd {::accept(fd(),nullptr, nullptr)};
        if(!client_fd.valid()){
            return std::unexpected(std::error_code{errno,std::system_category()});
        }

        return Socket{std::move(client_fd)};

    }

    std::expected<std::size_t, std::error_code>Socket::recv(std::span<std::byte> buffer) const {
        if(buffer.empty()) { return std::unexpected(std::make_error_code(std::errc::invalid_argument));}
        ssize_t received = ::recv(fd(), buffer.data(), buffer.size(), 0);
        if(received == -1){ return std::unexpected(std::error_code{errno,std::system_category()});}
        return static_cast<std::size_t>(received);
    }

    std::expected<std::size_t, std::error_code>Socket::send(std::span<const std::byte> buffer) const {
        if(buffer.empty()){ return std::unexpected(std::make_error_code(std::errc::invalid_argument));}
        ssize_t sent = ::send(fd(), buffer.data(), buffer.size(), 0);
        if(sent == -1){ return std::unexpected(std::error_code{errno,std::system_category()});}
        return static_cast<std::size_t>(sent);

    }

    std::expected<Socket, std::error_code> Socket::listen(std::string_view host, std::uint16_t port){

        FileDescriptor fd{::socket(AF_INET, SOCK_STREAM, 0)};
        if(!fd.valid()){
            return std::unexpected(std::error_code{errno, std::system_category()});
        }
        std::println("got file descriptor {}", fd.get());

        int yes = 1;
        if (::setsockopt(fd.get(), SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1){
            return std::unexpected(std::error_code{errno,std::system_category()});
        }

        sockaddr_in addr{};
        std::string host_str{host};
        addr.sin_family = AF_INET;
        addr.sin_port = ::htons(port);
        if(::inet_pton(AF_INET, host_str.c_str(), &addr.sin_addr) != 1){
            return std::unexpected(std::make_error_code(std::errc::invalid_argument));
        }


        if(::bind(fd.get(), reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1){
            return std::unexpected(std::error_code{errno, std::system_category()});
        }

        if(::listen(fd.get(), SOMAXCONN) == -1){
            return std::unexpected(std::error_code{errno, std::system_category()});
        }

        return Socket{std::move(fd)};
    }





}
