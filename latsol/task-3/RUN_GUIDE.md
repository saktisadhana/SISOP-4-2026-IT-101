# V-Protocol: HSS Recovery - Run Guide

## Prerequisites

These scripts **MUST** run on **Linux or WSL2 (Windows Subsystem for Linux)**. You cannot run them on Windows PowerShell directly.

### If you're on Windows 10/11:
1. Install WSL2: https://docs.microsoft.com/en-us/windows/wsl/install
2. Use Ubuntu (recommended): `wsl --install -d Ubuntu`
3. Open WSL terminal and navigate to the task-3 directory

### If you're on Linux:
Just open a terminal and navigate to the task-3 directory.

---

## Running the Scripts

### Step 1: Make scripts executable
```bash
chmod +x kernel.sh herta.sh curioDisc.sh save.sh
```

### Step 2: Run kernel.sh (Downloads & compiles Linux kernel v6.1.1)
**Time:** 30-60 minutes (depending on CPU)
```bash
sudo ./kernel.sh
```

### Step 3: Run herta.sh (Creates RootFS with BusyBox)
```bash
sudo ./herta.sh
```

### Step 4: Run curioDisc.sh (Creates bootable ISO)
```bash
sudo ./curioDisc.sh
```

### Step 5: Run save.sh (Archives and cleans up)
```bash
sudo ./save.sh
```

---

## Expected Output

After all scripts complete, you should have:
- ✅ `v_protocol_HSS.zip` - Final archive containing all components
- ✅ `kernel.sh`, `herta.sh`, `curioDisc.sh`, `save.sh` - The scripts
- ✅ Everything else cleaned up

---

## Testing with QEMU (Optional)

To test the ISO with QEMU emulator:

```bash
# Install QEMU
sudo apt install qemu-system-x86

# Run the ISO
qemu-system-x86_64 -cdrom aha.iso -m 512 -enable-kvm
```

### Testing checklist:

**Single Mode (First boot option):**
- [ ] Should display: `HERTA STATION: EMERGENCY ACCESS MODE ACTIVATED.`
- [ ] Should get root prompt: `/ #`

**Multi Mode (Second boot option):**
- [ ] Should show login prompt
- [ ] Login with `nous` / `nous123`
- [ ] Check secret file: `cat /home/nous/secret.txt`
- [ ] Should output: `Herta's Secret: Silver Wolf actually sucks at osu!`

---

## Troubleshooting

### Error: "apt command not found"
→ You're not on Linux/WSL. Use WSL2.

### Error: "bzImage not found"
→ Run `kernel.sh` first before running other scripts.

### Error: Permission denied
→ Make sure you're using `sudo` for scripts 2-5.

### Compilation takes too long?
→ That's normal! Kernel compilation takes 30-60 minutes. Don't interrupt it.

### extglob error in save.sh?
→ Make sure it runs with bash, not sh: `bash save.sh` or `sudo bash save.sh`

---

## Files Explained

| File | Purpose |
|------|---------|
| `kernel.sh` | Downloads & compiles Linux kernel v6.1.1 → produces `bzImage` |
| `herta.sh` | Creates root filesystem with BusyBox → produces `RootFS.gz` |
| `curioDisc.sh` | Creates bootable ISO with GRUB → produces `aha.iso` |
| `save.sh` | Archives everything into ZIP and cleans up workspace |

---

## All Fixed! 🎉

All scripts now have:
✅ Error handling (`set -e`)
✅ Status messages for debugging
✅ Pre-checks for required files
✅ Proper password handling
✅ Working init system for single/multi-user modes
✅ Proper extglob cleanup
