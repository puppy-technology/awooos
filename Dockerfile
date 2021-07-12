FROM alpine:3.9
# Using Alpine 3.9 because it's the only way I could manage to get Clang 5.x.

ENV AWOO_MAKE_CONFIG="config.mk.dist"
ENV QEMU_FLAGS="-display none"
# ENV MAKEFLAGS="-j 2"

RUN apk add --no-cache binutils clang lld git llvm5 make nasm qemu qemu-system-i386 syslinux xorriso
RUN apk add --no-cache wget unzip
