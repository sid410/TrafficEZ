#!/bin/bash

set -e

echo -e "\n\n-----------------------------------"
echo "##### Adding Missing APT Keys #####"
echo -e "-----------------------------------"

keys=(
    "40976EAF437D05B5"
    "3B4FE6ACC0B21F32"
)

temp_keyring="/tmp/temp-keyring.gpg"

echo "Creating a temporary keyring at $temp_keyring"
touch "$temp_keyring"

for key in "${keys[@]}"; do
    echo "Fetching and adding key: $key"
    gpg --no-default-keyring --keyring "$temp_keyring" --keyserver keyserver.ubuntu.com --recv-keys "$key"
    gpg --no-default-keyring --keyring "$temp_keyring" --export "$key" | sudo tee /etc/apt/trusted.gpg.d/"$key".gpg > /dev/null
done

echo "Cleaning up temporary keyring"
rm -f "$temp_keyring"

sudo apt update

echo "##### Keys Added and Packages Updated #####"
