#!/bin/bash

# This script elevates all bash scripts inside the same directory

current_directory="$(pwd)"
chmod +x "$current_directory"/*.sh
echo "Permissions updated for all Bash scripts in $current_directory."