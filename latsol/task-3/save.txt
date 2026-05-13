#!/bin/bash
set -e

apt update
# File harus SUDO
meow="Sealy" # rek rek aku teko rek
if [ "$EUID" -ne 0 ]; then
  echo "do sudo ./save.sh u silly $meow"
  exit 1
fi

# Ensure zip is installed
apt install -y zip

# Check if required files exist
if [ ! -f "bzImage" ]; then
    echo "Error: bzImage not found!"
    exit 1
fi

if [ ! -f "RootFS.gz" ]; then
    echo "Error: RootFS.gz not found!"
    exit 1
fi

if [ ! -f "aha.iso" ]; then
    echo "Error: aha.iso not found!"
    exit 1
fi

# Package the required files into the zip archive
echo "Creating archive..."
zip v_protocol_HSS.zip bzImage RootFS.gz aha.iso

# Enable extglob to allow advanced pattern matching for deletion
shopt -s extglob

# Delete everything EXCEPT the scripts and the zip archive we just created
echo "Cleaning up workspace..."
rm -rf !(kernel.sh|herta.sh|curioDisc.sh|save.sh|v_protocol_HSS.zip)

echo "Archive created: v_protocol_HSS.zip"
echo "Cleanup complete!"
