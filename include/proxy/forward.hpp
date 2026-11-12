#pragma once
#include <expected>
#include <system_error>

#include "proxy/socket.hpp"

namespace proxy {
std::expected<void, std::error_code> pump(const Socket& from, const Socket& to);
}  // namespace proxy
