#!/bin/bash
set -e

apt update

# File harus SUDO
meow="Sealy" # rek rek aku teko rek
if [ "$EUID" -ne 0 ]; then
  echo "do sudo ./herta.sh u silly $meow"
  exit 1
fi

# Install libnewt untuk membuat menu teks, dan busybox-static untuk RootFS
apt install -y libnewt-dev busybox-static

#
#   Main logic setup  #
#
# ADDED /sbin and /root here:
mkdir -p RootFS/{bin,sbin,dev,etc,home,proc,sys,usr/bin,usr/sbin,root}

# Gunakan busybox static untuk menyediakan semua command dasar (ls, cat, rm, dll)
cp /bin/busybox RootFS/bin/

# FIX: Removed the "/bin" at the end so it correctly populates /sbin and /usr/bin too
chroot RootFS /bin/busybox --install -s 

# Copy essential device nodes
cp -a /dev/{null,console,tty,zero} RootFS/dev/

#
#  INIT The OS Maw!  #
#
# I wont gave EEFFOC until i get my coffee
cat << 'EOF' > RootFS/init 
#!/bin/sh
mount -t proc none /proc 2>/dev/null || true
mount -t sysfs none /sys 2>/dev/null || true
mount -t devtmpfs none /dev 2>/dev/null || true

# Check if "single" was passed from GRUB
if grep -q "single" /proc/cmdline; then
    echo "HERTA STATION: EMERGENCY ACCESS MODE ACTIVATED."
    exec /bin/sh
else
    # Otherwise, boot into multi-user mode with login
    exec /sbin/init
fi
EOF
chmod +x RootFS/init

# create the dir for multi user
touch RootFS/etc/passwd 
touch RootFS/etc/shadow 
touch RootFS/etc/group
touch RootFS/etc/fstab # FIX: Added an empty fstab so 'mount -a' doesn't complain

# shadow not for single mode
chmod 600 RootFS/etc/shadow

# Create a default root user
echo "root:x:0:0:root:/root:/bin/sh" > RootFS/etc/passwd
echo "root:!:0:0:99999:7:::" > RootFS/etc/shadow
echo "root:x:0:" > RootFS/etc/group

# Create user nous
chroot RootFS /bin/sh -c 'adduser -D -s /bin/sh nous' 2>/dev/null || true

# Set password nous (using proper shadow entry format)
chroot RootFS /bin/sh -c 'echo "nous:nous123" | chpasswd' 2>/dev/null || true

# Create Madame Herta's secret file
mkdir -p RootFS/home/nous
echo "Herta's Secret: Silver Wolf actually sucks at osu!" > RootFS/home/nous/secret.txt
chmod 644 RootFS/home/nous/secret.txt

# Ensure the user 'nous' actually owns their home folder
chroot RootFS /bin/sh -c 'chown -R nous:nous /home/nous' 2>/dev/null || true

# Make TTY login prompt appear in Multi-User Mode
cat << 'EOF' > RootFS/etc/inittab
::sysinit:/bin/mount -t proc proc /proc
::sysinit:/bin/mount -t sysfs sysfs /sys
::sysinit:/bin/mount -t devtmpfs devtmpfs /dev
::respawn:/sbin/getty -L 115200 tty0 vt100
::ctrlaltdel:/sbin/reboot
::shutdown:/bin/umount -a -r
EOF

#
#  FINAL PACKAGING (REQUIRED FOR QEMU)  #
#
cd RootFS
find . -print0 | cpio --null -ov --format=newc | gzip -9 > ../RootFS.gz
cd ..
echo "RootFS packaging complete!"