#!/bin/bash

# Directories and log file setup
root_dir="$(pwd)/.."
resources_dir="$root_dir/resources/"
logs_root_dir="$root_dir/logs/"
logs_dir="$logs_root_dir/$(date + "%Y%m%d")/"
log_file="$logs_dir/monitor_$(date +"%Y%m%d_%H%M%S").log"

# Ensure the logs directory exists
mkdir -p "$logs_dir"

# Application command
app_cmd="$resources_dir/TrafficEZ -v"

# PID of the running application
pid=""

# Function to handle termination signals
cleanup() {
    echo "Stopping TrafficEZ..." | tee -a "$log_file"
    if [[ -n "$pid" ]]; then
        kill -SIGINT "$pid"  # Forward SIGINT to TrafficEZ
        wait "$pid"          # Wait for the application to exit
    fi
    echo "TrafficEZ stopped." | tee -a "$log_file"
    exit 0
}

# Trap SIGINT and SIGTERM signals
trap cleanup SIGINT SIGTERM

# Start the application with logging
echo "Starting TrafficEZ..." | tee -a "$log_file"
$app_cmd 2>&1 | tee -a "$log_file" &
pid=$!  # Save the PID of the background process

# Wait for the application to finish
wait "$pi