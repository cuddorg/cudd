FROM ghcr.io/almalinux/almalinux:8
ARG TARGETARCH TARGETOS TARGETPLATFORM TARGETVARIANT

RUN dnf install -y \
      dnf-plugins-core \
      dnf-plugin-config-manager \
      epel-release \
      && \
    dnf config-manager --enable powertools && \
    dnf install -y \
      git \
      wget \
      make \
      cmake \
      ninja-build \
      gcc-toolset-14-gcc \
      gcc-toolset-14-gcc-c++ \
      python3.12 \
      python3.12-pip \
      pkgconf-pkg-config \
      && \
    dnf clean all

RUN dnf install -y \
      clang \
      clang-devel \
      clang-tools-extra \
      clang-analyzer \
      lcov \
      valgrind \
      && \
    dnf clean all

RUN echo "source /opt/rh/gcc-toolset-14/enable" >> /etc/bashrc
