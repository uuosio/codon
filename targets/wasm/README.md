In the codon source directory, run the following command to build llvm

```bash
git clone --depth 1 -b codon https://github.com/exaloop/llvm-project
cmake -S llvm-project/llvm -B llvm-project/build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DLLVM_INCLUDE_TESTS=OFF \
    -DLLVM_ENABLE_RTTI=ON \
    -DLLVM_ENABLE_ZLIB=OFF \
    -DLLVM_ENABLE_TERMINFO=OFF \
    -DLLVM_TARGETS_TO_BUILD=all
cmake --build llvm-project/build
```

Build codon with the wasm runtime

- download wasi-sdk from https://github.com/WebAssembly/wasi-sdk/tags
- run the following command to build with codon wasm runtime

```bash
cmake -S . -B build -G Ninja \
    -DBUILD_WASM=TRUE \
    -DWASI_SDK_PREFIX=path/to/wasi-sdk \
    -DCMAKE_BUILD_TYPE=Release \
    -DLLVM_DIR=llvm-project/build \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++
```