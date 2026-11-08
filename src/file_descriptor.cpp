#include "proxy/file_descriptor.hpp"

#include <unistd.h>

#include <utility>

namespace proxy {

FileDescriptor::FileDescriptor(int fd) noexcept : fd_(fd) {}

int FileDescriptor::get() const noexcept { return fd_; }

bool FileDescriptor::valid() const noexcept { return fd_ != -1; }

FileDescriptor::FileDescriptor(FileDescriptor&& other) noexcept
    : fd_{std::exchange(other.fd_, -1)} {}

FileDescriptor& FileDescriptor::operator=(FileDescriptor&& other) noexcept {
    if (this != &other) {
        reset();
        fd_ = std::exchange(other.fd_, -1);
    }
    return *this;
}

void FileDescriptor::reset() noexcept {
    // -1 means it was never init or moved, no need to waste a sys call
    if (fd_ != -1) {
        close(fd_);
    }
}

FileDescriptor::~FileDescriptor() { reset(); }
}  // namespace proxy
