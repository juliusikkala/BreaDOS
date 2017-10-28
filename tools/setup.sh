#! /bin/bash

cd "${0%/*}"

GCC="gcc-7.2.0"
BINUTILS="binutils-2.29"
GRUB="grub-2.02~rc2"

PREFIX=$PWD
PATH="$PREFIX/bin:$PATH"
export PATH

if [ ! -e ${BINUTILS} ]; then
    wget -nH --cut-dirs=2 -m ftp://ftp.gnu.org/gnu/binutils/${BINUTILS}.tar.xz -O binutils
    tar -xJf binutils
    rm binutils
    
    mkdir build-binutils
    pushd build-binutils
    ../${BINUTILS}/configure --target=i686-elf --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
    make -j32
    make install
    popd
    rm -rf build-binutils

    mkdir build-binutils
    pushd build-binutils
    ../${BINUTILS}/configure --target=x86_64-elf --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
    make -j32
    make install
    popd
    rm -rf build-binutils
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
    ../${GCC}/configure --target=i686-elf --prefix="$PREFIX" --disable-nls --enable-languages=c --without-headers
    make all-gcc -j32
    make all-target-libgcc -j32
    make install-gcc
    make install-target-libgcc
    popd
    rm -rf build-gcc

    mkdir build-gcc
    pushd build-gcc
    ../${GCC}/configure --target=x86_64-elf --prefix="$PREFIX" --disable-nls --enable-languages=c --without-headers
    make all-gcc -j32
    make all-target-libgcc -j32
    make install-gcc
    make install-target-libgcc
    popd
    rm -rf build-gcc
fi

if [ ! -e ${GRUB} ]; then
    wget -nH --cut-dirs=2 -m ftp://alpha.gnu.org/gnu/grub/${GRUB}.tar.xz -O grub
    tar -xJf grub
    rm grub

    pushd ${GRUB}
    ./autogen.sh
    popd

    mkdir build-grub
    pushd build-grub
    ../${GRUB}/configure
    make dist -j32
    cp ${GRUB}.tar.gz ..
    tar -zxvf ${GRUB}.tar.gz
    pushd ${GRUB}
    ./configure --prefix="$PREFIX"
    make -j32
    make install
    popd
    popd
    rm -rf build-grub
fi

PREFIX_ESC=$(echo $PWD | sed 's_/_\\/_g')

sed -e "s/\\\$PREFIX/${PREFIX_ESC}/g" cross_file.template > ../cross_file.txt
