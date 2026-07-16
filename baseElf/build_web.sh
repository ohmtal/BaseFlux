#!bin/sh
cmake -S . -B build_web \
    -DCMAKE_TOOLCHAIN_FILE=/usr/lib/emscripten/cmake/Modules/Platform/Emscripten.cmake \
    -DCMAKE_BUILD_TYPE=Release
    # -DCMAKE_BUILD_TYPE=Release -DPLATFORM=Web
cmake --build build_web -j 4
