## Build demo

setup the build env

```bash
export WASI_SDK_PREFIX=path/to/wasi-sdk
export LLVM_BUILD_DIR=path/to/llvm-project/build
```

```bash
codon build --release -o hello.ll hello.codon
$LLVM_BUILD_DIR/bin/llc -march=wasm32 -filetype=obj hello.ll -o hello.o
$WASI_SDK_PREFIX/bin/clang -O3 -c --target=wasm32-wasi -nostdlib -o main.o main.c
$WASI_SDK_PREFIX/bin/wasm-ld --entry _start -L../../../build/targets/wasm/runtime-wasi -L$WASI_SDK_PREFIX/share/wasi-sysroot/lib/wasm32-wasi -lrt -lc++ -lc++abi -lc $WASI_SDK_PREFIX/share/wasi-sysroot/lib/wasm32-wasi/crt1.o -L$WASI_SDK_PREFIX/lib/clang/15.0.6/lib/wasi -lclang_rt.builtins-wasm32 --allow-undefined -o main.wasm main.o hello.o
```

## Run demo
```
wasmtime main.wasm
```
