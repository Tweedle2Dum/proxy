# proxy

A fast, lightweight L4 TCP proxy for Linux, written in modern C++23.

> **Status:** early development. Not ready for production use.

## Features

- Event-driven core built on `epoll`, scaling across cores
- Load balancing with round robin and least-connections strategies
- Active health checks with automatic backend failover
- Backpressure handling for slow clients and backends
- Correct TCP semantics: half-close, idle and connect timeouts
- Zero-copy forwarding via `splice`, with an experimental `io_uring` backend
- PROXY protocol v1/v2 to preserve client addresses
- TLS SNI-based routing without terminating TLS
- Graceful shutdown and live config reload
- Metrics endpoint

## Roadmap

- [x] Build system with sanitizer support
- [x] Socket primitives and RAII resource management
- [ ] Blocking forwarder
- [ ] `epoll` event loop
- [ ] Backpressure, half-close and timeouts
- [ ] Multi-core scaling
- [ ] Load balancing and health checks
- [ ] Zero-copy forwarding
- [ ] PROXY protocol and SNI routing
- [ ] Config reload, metrics, graceful shutdown
- [ ] Benchmarks

## Requirements

- Linux
- clang 17+ or GCC 14+ with C++23 support
- CMake 3.28+
- Ninja

## Building

```bash
git clone https://github.com/<user>/proxy.git
cd proxy
cmake --preset debug
cmake --build --preset debug
```

Available presets:

| Preset | Description |
|---|---|
| `debug` | Debug build with full debug info |
| `asan` | Debug build with AddressSanitizer and UndefinedBehaviorSanitizer |

## Design

- **No hidden allocations on the hot path.** Buffers are sized up front and reused.
- **RAII for every resource.** File descriptors, buffers and connections clean up after themselves.
- **Errors are values.** System call failures are returned as `std::expected`, not thrown.
- **Linux-first.** Uses Linux-specific APIs where they offer real performance benefits.

## Contributing

Issues and pull requests are welcome. Please run the `asan` build before submitting changes.

## License

MIT
