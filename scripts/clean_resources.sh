#!/bin/bash

root_dir="$(pwd)/.."
resources_dir="$root_dir/resources/"

# Check if the resources directory exists
if [ ! -d "$resources_dir" ]; then
  echo "Directory $resources_dir does not exist."
  exit 1
fi

# Find and delete all files except .gitignore
find "$resources_dir" -type f ! -name '.gitignore' -exec rm -f {} +

echo "Cleaned all files in $resources_dir"
