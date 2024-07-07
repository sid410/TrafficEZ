#!/bin/bash

# Define root and resources directories
root_dir="$(pwd)/.."
resources_dir="$root_dir/resources"

# Execute clean_resources.sh
./clean_resources.sh

# Check if clean_resources.sh executed successfully
if [ $? -ne 0 ]; then
  echo "clean_resources.sh failed. Exiting."
  exit 1
fi

# Define source files and directories
yolo_model_file="$root_dir/sample_configs/yolov8n-seg.onnx"
test_watcher_binaries_dir="$root_dir/test/test_watcher_binaries"

# Check if the files exist
if [ ! -f "$yolo_model_file" ]; then
  echo "File $yolo_model_file does not exist. Exiting."
  exit 1
fi

if [ ! -d "$test_watcher_binaries_dir" ]; then
  echo "Directory $test_watcher_binaries_dir does not exist. Exiting."
  exit 1
fi

# Copy the yolo model
cp "$yolo_model_file" "$resources_dir"

# Copy all files from test_watcher_binaries directory
cp "$test_watcher_binaries_dir"/* "$resources_dir"

echo "Copied files into $resources_dir"
