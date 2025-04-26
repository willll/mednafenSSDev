FROM debian:testing-20250317 AS build

ARG MXE_TARGET=x86_64-w64-mingw32.static
ARG MXE_PKG=mxe-x86-64-w64-mingw32.shared

WORKDIR /opt/mxe-w64/

# install dependencies
RUN apt-get update && \
    apt-get install -y \
    autoconf \
    automake \
    autopoint \
    bash \
    bison \
    bzip2 \
    cmake \
    flex \
    g++ \
    g++-multilib \
    gettext \
    git \
    gperf \
    intltool \
    libc6-dev-i386 \
    libgdk-pixbuf-xlib-2.0-dev \
    libltdl-dev \
    libgl-dev \
    libssl-dev \
    libtool-bin \
    libxml-parser-perl \
    lzip \
    make \
    openssl \
    p7zip-full \
    patch \
    perl \
    python3 \
    python3-distutils-extra \
    python3-mako \
    python3-pip \
    python3-pkg-resources \
    python-is-python3 \
    ruby \
    scons \
    sed \
    unzip \
    wget \
    xz-utils

# install mxe
RUN git clone --depth 1 https://github.com/mxe/mxe/ . && \
    make -j8 bfd libiberty flac sdl2 zlib glew freeglut cmake libiconv MXE_USE_CCACHE= MXE_TARGETS='x86_64-w64-mingw32.static' && \
    rm logs .ccache pkg -rf
    

# small runner
FROM debian:testing-20250317 AS runner
RUN apt-get update && \
    apt-get install -y \
    bzip2 \
    cmake \
    g++ \
    g++-multilib \
    gettext \
    git \
    gperf \
    intltool \
    libc6-dev-i386 \
    lzip \
    make \
    openssl \
    p7zip-full \
    python3 \
    python3-distutils-extra \
    python3-pip \
    unzip \
    wget \
    xz-utils \
    && rm -rf /var/lib/apt/lists/*
COPY --from=build /opt/mxe-w64 /opt/mxe-w64