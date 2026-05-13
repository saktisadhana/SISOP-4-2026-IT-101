#!/bin/bash
set -e

apt update
# File harus SUDO
meow="Sealy" # rek rek aku teko rek
if [ "$EUID" -ne 0 ]; then
  echo "do sudo ./curioDisc.sh u silly $meow"
  exit 1
fi

# Installing ISO Maker tools (grub-mkrescue dependencies, including amd64 modules for ARM Mac hosts)
apt install -y grub-common grub-pc-bin xorriso mtools grub-efi-amd64-bin

# Check if required files exist
if [ ! -f "bzImage" ]; then
    echo "Error: bzImage not found! Run kernel.sh first."
    exit 1
fi

if [ ! -f "RootFS.gz" ]; then
    echo "Error: RootFS.gz not found! Run herta.sh first."
    exit 1
fi

# Create the directory structure for the ISO
mkdir -p iso/boot/grub

# Copy the Kernel and RootFS into the ISO boot folder
# (bzImage was created by kernel.sh, RootFS.gz was created by herta.sh)
cp bzImage iso/boot/
cp RootFS.gz iso/boot/

# Create the GRUB boot menu configuration
cat << 'EOF' > iso/boot/grub/grub.cfg
set timeout=10
set default=0

menuentry "Emergency Shell (Single Mode)" {
    # The 'single' parameter triggers our custom Single Mode in RootFS/init
    linux /boot/bzImage single console=ttyS0 console=tty0 root=/dev/ram0 rw
    initrd /boot/RootFS.gz
}

menuentry "HSS Secure Login (Multi Mode)" {
    # Normal boot triggers Multi Mode login
    linux /boot/bzImage console=ttyS0 console=tty0 root=/dev/ram0 rw
    initrd /boot/RootFS.gz
}
EOF

# Build the final bootable ISO image
echo "Building ISO image..."
grub-mkrescue -o aha.iso iso/ || { echo "ISO creation failed!"; exit 1; }

# Clean up the temporary iso directory
rm -rf iso/
echo "ISO creation complete! File: aha.iso"