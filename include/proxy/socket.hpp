#pragma once

#include "proxy/file_descriptor.hpp"
#include <cstddef>
#include <cstdint>
#include <expected>
#include <span>
#include <string_view>
#include <system_error>


namespace proxy {
class Socket{

    public:

        [[nodiscard]]static std::expected<Socket, std::error_code> listen(std::string_view host, std::uint16_t port);

        [[nodiscard]] std::expected<Socket, std::error_code> accept() const;
        [[nodiscard]] std::expected<std::size_t, std::error_code> send(std::span<const std::byte> buffer) const;
        [[nodiscard]] std::expected<std::size_t, std::error_code> recv(std::span<std::byte> buffer) const;

        [[nodiscard]] int fd() const noexcept;


    private:
        explicit Socket(FileDescriptor fd) noexcept;
        FileDescriptor fd_;

};

}
