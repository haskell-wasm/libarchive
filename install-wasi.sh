#!/usr/bin/env bash

set -euo pipefail

WASI_SDK_PREFIX="${WASI_SDK_PREFIX:-/workspace/.ghc-wasm/wasi-sdk}"
WASI_PREFIX="${WASI_PREFIX:-/tmp/wasi}"

cmake \
  -Bbuild-wasi \
  -DCMAKE_TOOLCHAIN_FILE="$WASI_SDK_PREFIX/share/cmake/wasi-sdk.cmake" \
  -DCMAKE_EXE_LINKER_FLAGS="-Wl,--error-limit=0,--keep-section=target_features,--stack-first,--strip-debug,--lto-O3" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON \
  -DCMAKE_INSTALL_PREFIX="$WASI_PREFIX" \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCMAKE_MACOSX_RPATH=OFF \
  -DBUILD_SHARED_LIBS=OFF \
  -DENABLE_OPENSSL=OFF \
  -DENABLE_LIBB2=OFF \
  -DENABLE_LZ4=OFF \
  -DENABLE_LZMA=OFF \
  -DENABLE_ZSTD=ON \
  -DZSTD_INCLUDE_DIR="$WASI_PREFIX/include" \
  -DZSTD_LIBRARY="$WASI_PREFIX/lib/libzstd.a" \
  -DENABLE_ZLIB=OFF \
  -DENABLE_BZip2=OFF \
  -DENABLE_LIBXML2=OFF \
  -DENABLE_EXPAT=OFF \
  -DENABLE_WIN32_XMLLITE=OFF \
  -DENABLE_PCREPOSIX=OFF \
  -DENABLE_PCRE2POSIX=OFF \
  -DENABLE_CNG=OFF \
  -DENABLE_TAR=ON \
  -DENABLE_CPIO=OFF \
  -DENABLE_CAT=OFF \
  -DENABLE_UNZIP=OFF \
  -DENABLE_XATTR=OFF \
  -DENABLE_ACL=OFF \
  -DENABLE_ICONV=OFF \
  -DENABLE_TEST=OFF \
  -G Ninja \
  ${1+"$@"}

cmake --build build-wasi --target install -- -v

wasm-opt --debuginfo --low-memory-unused --strip-dwarf -O4 --converge "$WASI_PREFIX/bin/bsdtar" -o "$WASI_PREFIX/bin/bsdtar.wasm"

rm "$WASI_PREFIX/bin/bsdtar"

wasmtime run -- "$WASI_PREFIX/bin/bsdtar.wasm" --help
