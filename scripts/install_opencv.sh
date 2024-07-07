#!/bin/bash

set -e

echo -e "\n\n------------------------------------------"
echo "##### Downloading OpenCV source code #####"
echo -e "------------------------------------------"

# Clone the OpenCV repository with the specified version
version='4.9.0'
git clone https://github.com/opencv/opencv.git --branch ${version} --depth 1

# CMake command with the specified settings
cmakeCmd="cmake -S opencv -B opencv/build \
    -D CMAKE_BUILD_TYPE=RELEASE \
    -D CMAKE_CXX_COMPILER=g++ \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D WITH_TBB=ON \
    -D WITH_IPP=ON \
    -D BUILD_NEW_PYTHON_SUPPORT=OFF \
    -D WITH_V4L=ON \
    -D ENABLE_PRECOMPILED_HEADERS=ON \
    -D INSTALL_C_EXAMPLES=OFF \
    -D INSTALL_PYTHON_EXAMPLES=OFF \
    -D BUILD_EXAMPLES=OFF \
    -D WITH_QT=OFF \
    -D WITH_OPENGL=OFF \
    -D OPENCV_GENERATE_PKGCONFIG=ON"

# Output the CMake command for logging purposes
echo "Compile cmd: ${cmakeCmd}"

echo "##### Compiling and Installing OpenCV ${version} #####"

# Execute the CMake command
eval ${cmakeCmd}

# Build and install OpenCV using all available cores
num_cores=$(nproc)
make -j${num_cores} -C opencv/build
sudo make -C opencv/build install
sudo ldconfig

# Check if OpenCV installed correctly
if command -v opencv_version &> /dev/null
then
    echo "OpenCV installed successfully"
    # Clean up the opencv folder after installation
    echo "Cleaning up..."
    rm -rf opencv
    echo "OpenCV source folder has been cleaned up."
else
    echo "OpenCV installation failed"
    exit 1
fi