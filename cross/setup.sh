#! /bin/bash

cd "${0%/*}"

GCC="gcc-7.2.0"
BINUTILS="binutils-2.29"

PREFIX=$(pwd)
TARGET=i686-elf
PATH="$PREFIX/bin:$PATH"

if [ ! -e ${BINUTILS} ]; then
    wget -nH --cut-dirs=2 -m ftp://ftp.gnu.org/gnu/binutils/${BINUTILS}.tar.xz -O binutils
    tar -xJf binutils
    rm binutils
    
    mkdir build-binutils
    pushd build-binutils
    ../${BINUTILS}/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
    make
    make install
    popd
fi

if [ ! -e ${GCC} ]; then
    wget -nH --cut-dirs=3 -m ftp://ftp.gnu.org/gnu/gcc/${GCC}/${GCC}.tar.xz -O gcc
    tar -xJf gcc
    rm gcc

    pushd ${GCC}
    contrib/download_prerequisites
    popd

    mkdir build-gcc
    pushd build-gcc
    ../${GCC}/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c --without-headers
    make all-gcc
    make all-target-libgcc
    make install-gcc
    make install-target-libgcc
    popd
fi
