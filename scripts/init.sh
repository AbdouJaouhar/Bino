#!/bin/bash
set -e

echo "[*] Updating system..."
sudo apt-get update

echo "[*] Installing base build tools and libs..."
sudo apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    git \
    wget \
    gfortran \
    curl \
    zip \
    unzip \
    tar \
    ninja-build

echo "[*] Installing OpenCV development dependencies..."
sudo apt-get install -y \
    libopencv-dev \
    libjpeg-dev \
    libtiff-dev \
    libavcodec-dev \
    libavformat-dev \
    libswscale-dev \
    libv4l-dev \
    libxvidcore-dev \
    libx264-dev \
    libgtk2.0-dev \
    libgtk-3-dev \
    libatlas-base-dev \
    ffmpeg \
    libsm6 \
    libxext6 \
    libgl1 \
    mesa-utils

echo "[*] Installing Qt6..."
sudo apt-get install -y qt6-base-dev

echo "[*] Installing USB dependencies..."
sudo apt-get install -y libusb-1.0-0 libusb-1.0-0-dev usbutils

echo "[*] Creating uv environment..."
uv venv .venv
source .venv/bin/activate

echo "[*] Installing Python dependencies with uv..."
uv pip install opencv-python

if [ ! -d "depthai-core" ]; then
    echo "[*] Cloning depthai-core..."
    git clone https://github.com/luxonis/depthai-core.git depthai-core
fi

echo "[*] Updating depthai-core submodules..."
cd depthai-core
git submodule update --init --recursive
cd ..

echo "[*] Building depthai-core..."
cmake -S depthai-core -B depthai-build \
    -D CMAKE_BUILD_TYPE=Release \
    -D BUILD_SHARED_LIBS=ON \
    -D CMAKE_INSTALL_PREFIX=/usr/local

cmake --build depthai-build --parallel 4 --config Release
sudo cmake --install depthai-build

if [ ! -d "depthai-python" ]; then
    echo "[*] Cloning depthai-python..."
    git clone --recursive https://github.com/luxonis/depthai-python.git depthai-python
fi

echo "[*] Installing depthai-python with uv..."
cd depthai-python
uv pip install .
cd ..

echo "[*] Done. Local installation completed using uv."
