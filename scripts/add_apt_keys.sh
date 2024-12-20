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
    echo "Fetching and adding key: $key"
    curl -fsSL "https://keyserver.ubuntu.com/pks/lookup?op=get&search=0x$key" | gpg --dearmor | sudo tee /etc/apt/trusted.gpg.d/"$key".gpg > /dev/null
done

sudo apt update

echo "##### Keys Added and Packages Updated #####"
