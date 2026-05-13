# Project MOO — Dockerfile
# Base image: ubuntu:latest
FROM ubuntu:latest

# Set workdir /app
WORKDIR /app

# Copy server binary ke workdir
COPY server /app/server

# Buat folder db untuk mount point nanti
RUN mkdir -p /app/db

# Kasih permission execute ke server
RUN chmod +x /app/server

# Expose port 9000 untuk TCP connection
EXPOSE 9000

# Jalankan server saat container start
CMD ["./server"]
