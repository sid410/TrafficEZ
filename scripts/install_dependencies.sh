#!/bin/bash

set -e

echo -e "\n\n-----------------------------------"
echo "##### Installing Dependencies #####"
echo -e "-----------------------------------"

# Remove x264 and libx264-dev packages if they are already installed
sudo apt -y remove x264 libx264-dev

sudo apt -y install build-essential checkinstall cmake pkg-config yasm
sudo apt -y install git gfortran
sudo apt -y install libjpeg8-dev libpng-dev

sudo apt -y install software-properties-common
sudo add-apt-repository 'deb http://security.ubuntu.com/ubuntu xenial-security main'
sudo apt-get update

sudo apt -y install libjasper1 libjasper-dev
sudo apt -y install libtiff-dev

sudo apt -y install libavcodec-dev libavformat-dev libswscale-dev libdc1394-dev
sudo apt -y install libxine2-dev libv4l-dev
cd /usr/include/linux
sudo ln -s -f ../libv4l1-videodev.h videodev.h
cd "$cwd"

sudo apt -y install libunwind-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev
sudo apt -y install gstreamer1.0-tools gstreamer1.0-plugins-base gstreamer1.0-libav
sudo apt -y install gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly
sudo apt -y install libgtk2.0-dev libtbb-dev libcanberra-gtk-module libcanberra-gtk3-module
sudo apt -y install qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools
sudo apt -y install libatlas-base-dev
sudo apt -y install libfaac-dev libmp3lame-dev libtheora-dev
sudo apt -y install libvorbis-dev libxvidcore-dev
sudo apt -y install libopencore-amrnb-dev libopencore-amrwb-dev
sudo apt -y install x264 v4l-utils

sudo apt -y install libtbbmalloc2

sudo apt -y install python3-dev python3-numpy
sudo apt -y install curl libcurl4-openssl-dev

#cpprestskd dependencies
sudo apt -y install libboost-all-dev libwebsocketpp-dev openssl libssl-dev
sudo apt -y install libcpprest-dev

echo "##### Dependencies Installed #####"