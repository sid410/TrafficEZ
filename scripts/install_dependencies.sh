#!/bin/bash

set -e

echo -e "\n\n-----------------------------------"
echo "##### Installing Dependencies #####"
echo -e "-----------------------------------"

sudo apt -y remove x264 libx264-dev

sudo apt -y install build-essential checkinstall cmake pkg-config yasm git gfortran \
    libjpeg8-dev libpng-dev software-properties-common libjasper1 libjasper-dev \
    libtiff-dev libavcodec-dev libavformat-dev libswscale-dev libdc1394-dev libxine2-dev \
    libv4l-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgtk2.0-dev \
    libtbb-dev libcanberra-gtk-module libcanberra-gtk3-module qtbase5-dev qtchooser \
    qt5-qmake qtbase5-dev-tools libatlas-base-dev libfaac-dev libmp3lame-dev libtheora-dev \
    libvorbis-dev libxvidcore-dev libopencore-amrnb-dev libopencore-amrwb-dev x264 \
    v4l-utils libtbbmalloc2 python3-dev python3-numpy curl libcurl4-openssl-dev

sudo add-apt-repository "deb http://security.ubuntu.com/ubuntu xenial-security main"
sudo apt-get update

echo "##### Dependencies Installed #####"