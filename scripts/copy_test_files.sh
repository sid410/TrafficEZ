#!/bin/bash

root_dir="$(dirname "$(dirname "$(readlink -f "$0")")")"
resources_dir="$root_dir/resources"
clean_script="$root_dir/scripts/clean_resources.sh"

$clean_script

if [ $? -ne 0 ]; then
  echo "clean_resources.sh failed. Exiting."
  exit 1
fi

yolo_model_file="$root_dir/sample_configs/yolov8n-seg.onnx"
test_watcher_binaries_dir="$root_dir/test/test_watcher_binaries"

if [ ! -f "$yolo_model_file" ]; then
  echo "File $yolo_model_file does not exist. Exiting."
  exit 1
fi

if [ ! -d "$test_watcher_binaries_dir" ]; then
  echo "Directory $test_watcher_binaries_dir does not exist. Exiting."
  exit 1
fi

cp "$yolo_model_file" "$resources_dir"
cp "$test_watcher_binaries_dir"/* "$resources_dir"

echo "Copied files into $resources_dir"