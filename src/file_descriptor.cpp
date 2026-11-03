#include "proxy/file_descriptor.hpp"
#include <unistd.h>
#include <utility>


namespace proxy {

    FileDescriptor::FileDescriptor(int fd) noexcept : fd_(fd) {}

    int FileDescriptor::get() const noexcept{
        return fd_ ;
    }

    bool FileDescriptor::valid() const noexcept{
        return fd_ != -1;
    }

    FileDescriptor::FileDescriptor(FileDescriptor&& other) noexcept
        : fd_{std::exchange(other.fd_, -1)} {}

    FileDescriptor& FileDescriptor::operator=(FileDescriptor&& other) noexcept {
        if (this != &other){
            if(fd_ == -1) {
                close(fd_);
            }
            fd_ = std::exchange(other.fd_, -1);
        }
        return *this;
    }
}
