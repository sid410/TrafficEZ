#!/bin/bash

export GTK_THEME=Adwaita:dark

root_dir="$(pwd)/.."
resources_dir="$root_dir/resources/"

# Start debugging (first found instance of TrafficEZ)
debug_file=$(find "$resources_dir" -type f -name "TrafficEZ-*" | head -n 1)

if [ -n "$debug_file" ]; then
    cd "$resources_dir"
    echo "Debugging: $debug_file"
    echo "=========="
    "$debug_file" -d
else
    echo "File to debug not found."
fi