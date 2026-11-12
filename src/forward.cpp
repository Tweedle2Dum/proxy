#include "proxy/forward.hpp"

#include <array>
#include <cstddef>
#include <expected>
#include <span>
#include <system_error>

#include "proxy/socket.hpp"

namespace proxy {

std::expected<void, std::error_code> pump(const Socket& from, const Socket& to) {
    std::array<std::byte, 16 * 1024> buffer{};
    while (true) {
        auto data = from.recv(buffer);
        if (!data) {
            return std::unexpected(data.error());
        }
        if (*data == 0) {
            // received 0, sender is done FIN, pass it on
            auto closed = to.shutdown_write();
            if (!closed) {
                return std::unexpected(closed.error());
            }
            return {};
        }
        auto sent = to.send_all(std::span{buffer}.first(*data));
        if (!sent) {
            return std::unexpected(sent.error());
        }
    }
};
}  // namespace proxy
