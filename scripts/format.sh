#!/bin/sh
cd "$(dirname "$0")/.." || exit 1
clang-format -i src/*.cpp include/proxy/*.hpp apps/*.cpp examples/*.cpp
