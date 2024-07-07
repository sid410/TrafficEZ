#!/bin/bash

read -sp "sudo password: " SUDO_PASS
echo

# URLs
BASE_URL="https://raw.githubusercontent.com/sid410/TrafficEZ/main"
SCRIPTS_URL="$BASE_URL/scripts"
CONFIGS_URL="https://api.github.com/repos/sid410/TrafficEZ/contents/sample_configs"
RELEASE_URL="https://api.github.com/repos/sid410/TrafficEZ/releases/latest"

# The array of scripts to download and run
scripts=("add_apt_keys.sh" "install_dependencies.sh" "install_opencv.sh")

INSTALL_DIR="$HOME/TrafficEZ_tmp"
RESOURCES_DIR="$HOME/TrafficEZ/resources"

# Ensure the directories exist
mkdir -p "$INSTALL_DIR"
mkdir -p "$RESOURCES_DIR"

log() {
    echo -e "[$(date +"%Y-%m-%d %H:%M:%S")] $1"
}

# Download the scripts into the temporary directory
for script in "${scripts[@]}"; do
    log "Downloading script: $script"
    if curl -o "$INSTALL_DIR/$script" "$SCRIPTS_URL/$script"; then
        chmod +x "$INSTALL_DIR/$script"
        log "Successfully downloaded and set executable: $script"
    else
        log "Failed to download: $script"
        exit 1
    fi
done

# Download the ORT shared library
log "Fetching latest release asset URL for libonnxruntime.so.1.17.1"
ASSET_URL=$(curl -s $RELEASE_URL | grep browser_download_url | grep libonnxruntime.so.1.17.1 | cut -d '"' -f 4)
log "Downloading latest release asset: libonnxruntime.so.1.17.1"
if curl -L -o "$INSTALL_DIR/libonnxruntime.so.1.17.1" "$ASSET_URL"; then
    log "Successfully downloaded: libonnxruntime.so.1.17.1"
else
    log "Failed to download: libonnxruntime.so.1.17.1"
    exit 1
fi

# Install the shared library globally
log "Installing libonnxruntime.so.1.17.1 to /usr/local/lib/"
if echo $SUDO_PASS | sudo -S cp "$INSTALL_DIR/libonnxruntime.so.1.17.1" /usr/local/lib/ && \
   echo $SUDO_PASS | sudo -S ldconfig; then
    log "Successfully installed libonnxruntime.so.1.17.1"
else
    log "Failed to install libonnxruntime.so.1.17.1"
    exit 1
fi

# Execute the scripts
for script in "${scripts[@]}"; do
    log "Executing script: $script"
    if echo $SUDO_PASS | sudo -S "$INSTALL_DIR/$script"; then
        log "Successfully executed: $script"
    else
        log "Failed to execute: $script"
        exit 1
    fi
done

# Download the TrafficEZ file from the latest release
log "Fetching TrafficEZ file URL from latest release"
TRAFFIC_EZ_URL=$(curl -s $RELEASE_URL | grep browser_download_url | grep TrafficEZ | cut -d '"' -f 4)
log "Downloading TrafficEZ"
if curl -L -o "$RESOURCES_DIR/TrafficEZ" "$TRAFFIC_EZ_URL"; then
    log "Successfully downloaded TrafficEZ"
else
    log "Failed to download TrafficEZ"
    exit 1
fi

# Download everything inside sample_configs
log "Fetching file list from sample_configs"
CONFIG_FILES=$(curl -s $CONFIGS_URL)
if [ -z "$CONFIG_FILES" ]; then
    log "Failed to fetch file list from sample_configs or directory is empty"
    exit 1
fi

echo "$CONFIG_FILES" | grep -oP '"download_url": "\K[^"]*' | while read -r file_url; do
    file_name=$(basename $file_url)
    log "Downloading $file_name from sample_configs"
    if curl -o "$RESOURCES_DIR/$file_name" "$file_url"; then
        log "Successfully downloaded: $file_name"
    else
        log "Failed to download: $file_name"
        exit 1
    fi
done

# Cleanup
log "Cleaning up installation directory: $INSTALL_DIR"
rm -rf "$INSTALL_DIR"

log "##### TrafficEZ Installed Successfully! #####"
log "cd ~/TrafficEZ/resources/"
log "Update the config files, then run ./TrafficEZ"