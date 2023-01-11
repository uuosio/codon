cmake_minimum_required(VERSION 3.16.3)

add_library( rt exc.cpp lib.cpp ../../../build/_deps/fmt-src/src/format.cc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -std=c++17")

target_compile_options(rt PRIVATE --sysroot=${WASI_SDK_PREFIX}/share/wasi-sysroot --target=wasm32-wasi)
# target_link_libraries(rt PRIVATE fmt::fmt)
target_include_directories(rt PRIVATE ../../../build/_deps/fmt-src/include)
