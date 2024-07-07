#!/bin/bash

set -e

echo -e "\n\n-----------------------------------"
echo "##### Adding Missing APT Keys #####"
echo -e "-----------------------------------"

sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 40976EAF437D05B5

sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 3B4FE6ACC0B21F32

sudo apt-get update

echo "#####"