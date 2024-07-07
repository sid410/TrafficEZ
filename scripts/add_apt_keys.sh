#!/bin/bash

set -e

echo -e "\n\n-----------------------------------"
echo "##### Adding Missing APT Keys #####"
echo -e "-----------------------------------"

keys=(
    "40976EAF437D05B5"
    "3B4FE6ACC0B21F32"
)

for key in "${keys[@]}"; do
    sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys "$key"
done

sudo apt-get update

echo "##### Keys Added and Packages Updated #####"
