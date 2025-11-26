FROM nvidia/cuda:12.4.1-devel-ubuntu22.04

# Based on https://github.com/luxonis/depthai-python/blob/main/ci/Dockerfile

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC

RUN apt-get update && apt-get install -y wget build-essential cmake pkg-config libjpeg-dev libtiff5-dev libavcodec-dev libavformat-dev libswscale-dev libv4l-dev libxvidcore-dev libx264-dev libgtk2.0-dev libgtk-3-dev libatlas-base-dev gfortran git libopencv-dev

RUN apt update && apt install python3-pip -y

ADD docker/docker_dependencies.sh .
RUN chmod +x docker_dependencies.sh && ./docker_dependencies.sh

RUN pip install -U pip && pip install --extra-index-url https://www.piwheels.org/simple/ --prefer-binary opencv-python

# Clone core
RUN git clone https://github.com/luxonis/depthai-core.git /depthai-core \
    && cd /depthai-core \
    && git submodule update --init --recursive

RUN apt-get update && apt-get install -y \
    curl \
    zip \
    unzip \
    tar \
    git \
    build-essential \
    cmake \
    && rm -rf /var/lib/apt/lists/*

# Build C++ library
RUN cmake -S /depthai-core -B /build \
    -D CMAKE_BUILD_TYPE=Release \
    -D BUILD_SHARED_LIBS=ON \
    -D CMAKE_INSTALL_PREFIX=/usr/local

RUN cmake --build /build --parallel 4 --config Release --target install

RUN git clone --recursive https://github.com/luxonis/depthai-python.git /depthai-python
RUN cd /depthai-python && python3 -m pip install .

RUN apt-get update && apt-get install -y \
    build-essential \
    ninja-build \
    cmake \
    git \
    && rm -rf /var/lib/apt/lists/*

# Qt6
RUN apt-get update && apt-get install -y \
    qt6-base-dev \
    && rm -rf /var/lib/apt/lists/*

# OpenCV
RUN apt-get update && apt-get install -y \
    libopencv-dev \
    && rm -rf /var/lib/apt/lists/*

RUN apt-get update && apt-get install -y libusb-1.0-0 libusb-1.0-0-dev
RUN apt-get update && apt-get install -y usbutils
WORKDIR /workspace
