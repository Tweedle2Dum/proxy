#pragma once

namespace proxy {

class FileDescriptor {
   public:
    FileDescriptor() = default;
    explicit FileDescriptor(int fd) noexcept;
    FileDescriptor(const FileDescriptor&) = delete;
    FileDescriptor& operator=(const FileDescriptor&) = delete;

    FileDescriptor(FileDescriptor&& other) noexcept;
    FileDescriptor& operator=(FileDescriptor&& other) noexcept;

    [[nodiscard]] int get() const noexcept;
    [[nodiscard]] bool valid() const noexcept;
    void reset() noexcept;
    ~FileDescriptor();

   private:
    int fd_ = -1;
};

}  // namespace proxy
