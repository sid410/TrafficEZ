#!/bin/bash

root_dir="$(pwd)/.."
resources_dir="$root_dir/resources/"
build_dir="$root_dir/build/"

# Clean the compiled binaries under resources
clean_compiled=$(find "$resources_dir" -type f -name "TrafficEZ-*")

if [ -n "$clean_compiled" ]; then
    find "$resources_dir" -type f -name "TrafficEZ-*" -exec rm -f {} \;
    echo "Cleaned compiled binaries in $resources_dir"
else
    echo "No binaries found to clean."
fi

# Transfer the current build to resources folder for testing
transfer_compiled=$(find "$build_dir/src" -type f -name "TrafficEZ-*")

if [ -n "$transfer_compiled" ]; then
    find "$build_dir/src" -type f -name "TrafficEZ-*" -exec cp {} "$resources_dir" \;
    echo "Transferred build files to $resources_dir"
else
    echo "No build found to transfer."
fi

# Delete the build folder, but wait for confirmation
read -p "Delete $build_dir? (y/n): " answer
if [ "$answer" == "y" ]; then
    rm -rf "$build_dir"
    echo "Build folder successfully deleted."
else
    echo "Delete command canceled."
fi
