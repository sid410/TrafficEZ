#!/bin/bash

# Define directories and log file paths
root_dir="$(pwd)/.."
resources_dir="$root_dir/resources/"
logs_root_dir="$root_dir/logs/"
logs_dir="$logs_root_dir/$(date +"%Y%m%d")/"
log_file="$logs_dir/debug_$(date +"%H%M%S").log"

# Ensure the logs directory exists
mkdir -p "$logs_dir"

# Start debugging (first found instance of TrafficEZ)
debug_file=$(find "$resources_dir" -type f -name "TrafficEZ-*" | head -n 1)

if [ -n "$debug_file" ]; then
    cd "$resources_dir"
    echo "Debugging: $debug_file"
    echo "==========" | tee -a "$log_file"
    "$debug_file" -d -v 2>&1 | tee -a "$log_file"
else
    echo "File to debug not found." | tee -a "$log_file"
fi
