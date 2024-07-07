#!/bin/bash

root_dir="$(dirname "$(dirname "$(readlink -f "$0")")")"
resources_dir="$root_dir/resources/"

if [ ! -d "$resources_dir" ]; then
  echo "Directory $resources_dir does not exist."
  exit 1
fi

find "$resources_dir" -type f ! -name '.gitignore' -exec rm -f {} +

echo "Cleaned all files in $resources_dir"