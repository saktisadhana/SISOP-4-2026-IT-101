FROM ubuntu:latest

# Install Samba
RUN apt-get update && apt-get install -y samba && rm -rf /var/lib/apt/lists/*

# Copy konfigurasi dan script entrypoint
COPY smb.conf /etc/samba/smb.conf
COPY entrypoint.sh /entrypoint.sh

# Beri izin eksekusi pada script
RUN chmod +x /entrypoint.sh

# Eksekusi script saat container berjalan
ENTRYPOINT ["/entrypoint.sh"]