#!/bin/bash

# 1. Membuat Group
groupadd readonly
groupadd staff

# 2. Membuat User & Memasukkannya ke Group
useradd -M -s /usr/sbin/nologin -G readonly member
useradd -M -s /usr/sbin/nologin -G staff contributor
useradd -M -s /usr/sbin/nologin -G staff librarian

# 3. Mengatur Password Samba
(echo "member123"; echo "member123") | smbpasswd -s -a member
(echo "contrib456"; echo "contrib456") | smbpasswd -s -a contributor
(echo "lib789"; echo "lib789") | smbpasswd -s -a librarian

# 4. Inisialisasi Direktori
mkdir -p /libraryit/ebooks /libraryit/papers /libraryit/sourcecode /libraryit/docs

# 5. Konfigurasi Permission Direktori (Syarat c)
# Ebooks & Papers: Bisa diakses staff
chown root:staff /libraryit/ebooks /libraryit/papers
chmod 775 /libraryit/ebooks /libraryit/papers

# Sourcecode: Pemilik dan grup saja (750)
chown root:staff /libraryit/sourcecode
chmod 750 /libraryit/sourcecode

# Docs: Read-only dari host, tapi writeable via Samba. 
# Dengan mengatur owner root:staff dan chmod 775, user host (biasanya UID 1000) 
# akan masuk ke kategori "others" (r-x) sehingga tidak bisa memodifikasi langsung.
chown root:staff /libraryit/docs
chmod 775 /libraryit/docs

# Menjalankan service Samba di foreground agar container tetap hidup
exec smbd -F --no-process-group