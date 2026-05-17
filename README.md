# SISOP-4-2026-IT-101

## Identitas Praktikan

| Nama                     | NRP        | Kode Asisten | Kelas |
| ------------------------ | ---------- | ------------ | ----- |
| Putu Putra Sakti Sadhana | 5027251101 | NINN         | A     |

## Reporting

### Soal 1: Kenz Rescue

Pada soal ini praktikan diperintahkan untuk membuat sebuah FUSE passthrough filesystem yang bisa membaca direktori target secara asinkron dan menggabungkan pecahan-pecahan petunjuk dari file `1.txt` hingga `7.txt` menjadi sebuah virtual file bernama `tujuan.txt`. 

#### Penjelasan

Sesuai dengan soal, praktikan membuat satu file program utama `kenz_rescue.c` untuk mengimplementasikan virtual file system FUSE.

##### kenz_rescue.c

File ini berisi kode FUSE yang berfungsi sebagai passthrough dan di saat yang sama menginjeksikan virtual file `tujuan.txt` ke level root FUSE mount.

###### Global Variables dan Virtual File Definition
```c
static char source_dir[PATH_MAX];

static const char *virtual_file_name = "tujuan.txt";
static const char *virtual_file_path = "/tujuan.txt";
```
Variabel `source_dir` digunakan untuk menyimpan path absolut tempat direktori *source* berada. String `virtual_file_name` dan `virtual_file_path` mendefinisikan lokasi spesifik file bayangan yang akan kita suntikkan (virtual file).

###### Ekstraksi Koordinat (get_tujuan_content)
```c
static void get_tujuan_content(char *buffer)
{
    char combined[1024] = "";
    for (int i = 1; i <= 7; i++)
    {
        char filepath[PATH_MAX];
        sprintf(filepath, "%s/%d.txt", source_dir, i);
        FILE *f = fopen(filepath, "r");
        if (f)
        {
            char line[256];
            while (fgets(line, sizeof(line), f))
            {
                char *pos = strstr(line, "KOORD: ");
                if (pos)
                {
                    pos += 7; // skip "KOORD: "
                    // bersihkan karakter newline (\n atau \r)
                    strcat(combined, pos);
                    break;
                }
            }
            fclose(f);
        }
    }
    sprintf(buffer, "Tujuan Mas Amba: %s\n", combined);
}
```
Fungsi pembantu ini bertugas membaca file teks bernomor `1.txt` hingga `7.txt` di dalam folder *source*. Jika file ditemukan, setiap baris file tersebut akan dicek apakah mengandung substring `"KOORD: "`. Jika ada, fungsi akan mengambil teks koordinat di depannya, membuang baris baru (newline), dan menambahkan teks tersebut ke string `combined`. Hasil akhir diformat dengan struktur kalimat `"Tujuan Mas Amba: ..."` dan disimpan di parameter `buffer`.

###### Callback: xmp_getattr
```c
static int xmp_getattr(const char *path, struct stat *stbuf)
{
    if (strcmp(path, virtual_file_path) == 0)
    {
        char buffer[2048];
        get_tujuan_content(buffer);
        stbuf->st_mode = S_IFREG | 0444; // Regular file, read-only
        stbuf->st_nlink = 1;
        stbuf->st_size = strlen(buffer);
        stbuf->st_uid = getuid();
        stbuf->st_gid = getgid();
        stbuf->st_atime = stbuf->st_mtime = stbuf->st_ctime = time(NULL);
        return 0;
    }
    // ... passthrough lstat untuk file lainnya
}
```
Ketika sistem (atau command `ls -l`) mencoba mengecek atribut file di filesystem FUSE, fungsi ini dipanggil. Jika user membaca atribut path `/tujuan.txt`, FUSE akan memanggil `get_tujuan_content` sekilas untuk mendapatkan *size* byte yang sesuai, lalu mengatur st_mode ke `S_IFREG` (file normal) dan `0444` (read-only untuk semua group) seakan-akan filenya ada, padahal kita membangkitkan stats nya di memory (Virtual). Path di luar `/tujuan.txt` diteruskan normal lewat fungsi `lstat`.

###### Callback: xmp_readdir
```c
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, ...)
{
    // ... read source directory (opendir, readdir) dan filler() semua isi asli
    
    // Inject virtual file di root directory FUSE
    if (strcmp(path, "/") == 0)
    {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_mode = S_IFREG | 0444;
        filler(buf, virtual_file_name, &st, 0);
    }
    return 0;
}
```
Untuk bisa memunculkan "tujuan.txt" saat user melakukan `ls`, fungsi ini menambahkan string virtual_file_name ke dalam *filler buf*. Karena kita hanya ingin ia berada di paling luar, modifikasi *filler* ini dibatasi hanya jika `path` yang dicek adalah root directory FUSE (`"/"`).

###### Callback: xmp_read
```c
static int xmp_read(const char *path, char *buf, size_t size, off_t offset, ...)
{
    if (strcmp(path, virtual_file_path) == 0)
    {
        char content_buffer[2048];
        get_tujuan_content(content_buffer);
        size_t len = strlen(content_buffer);
        
        // ... mencopy isi konten ke `buf` sesuai size dan offset
        return size;
    }
    // ... passthrough read (pread) untuk file lainnya
}
```
Saat user mencoba membuka file (misalnya menggunakan command `cat`), fungsi read dieksekusi. Jika file tersebut `tujuan.txt`, string data dari pergerakan `get_tujuan_content` akan direplikasi ke dalam target pointer `buf` sehingga pengguna menerima teks *on-the-fly*. Passthrough lain diarahkan menggunakan system call Unix standard `pread`.
#### Output
1. Melakukan Fuse 
![](Assets/Pasted%20image%2020260517215821.png)

![](Assets/Pasted%20image%2020260517215906.png)

2. ls -l
![](Assets/Pasted%20image%2020260517215934.png)

3. cat tujuan.txt
![](Assets/Pasted%20image%2020260517220001.png)

#### Kendala
Tidak ada kendala.

---

### Soal 2: Project MOO

Pada soal ini praktikan diminta membuat filesystem FUSE (*Encrypted Passthrough*) di mana setiap penulisan/pembuatan file akan dienkripsi secara otomatis dan dinamai dengan tambahan akhiran `.enc`. File kemudian dapat dibaca dengan didekripsi langsung secara on-the-fly. Kita juga mengoperasikan file *client.c* dan Docker yang terhubung untuk sebuah *Database Server TCP*.

#### Penjelasan

##### fuse.c

File `fuse.c` ini adalah implementasi sistem file enskripsi.

###### Helper: XOR Cipher Encryption & Decryption
```c
static void xor_cipher(char *data, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        data[i] ^= XOR_KEY; // XOR_KEY adalah 0x76
    }
}
```
Metode enkripsi yang digunakan pada soal ini adalah XOR (simetris) dengan kunci `0x76`. Keunggulan model XOR adalah logika enkripsi bisa digunakan sama persis untuk dekripsi.

###### Manipulasi Path dan Penyembunyian Ekstensi (.enc)
```c
static void get_enc_path(char *enc_path, const char *path)
{
    sprintf(enc_path, "%s%s.enc", source_dir, path);
}
```
File fisik tersimpan dengan format *.enc* (misal: "password.txt.enc"). Namun di lingkungan FUSE, kita tidak boleh memperlihatkan bahwa ekstensi itu ada.

Maka pada FUSE callback `xmp_readdir` kita melakukan pemangkasan nama di saat *listing* folder:
```c
// readdir
size_t len = strlen(display_name);
if (len > 4 && strcmp(display_name + len - 4, ".enc") == 0)
{
    display_name[len - 4] = '\0';
}
// masuk filler buffer
```

Sedangkan pada saat operasi Open, Read, Create, atau Write, kita gunakan *helper* yang secara gaib menempelkan ekstensi *.enc* saat memanggil call di sistem yang sebenarnya.

###### Callback: xmp_write (Auto Encrypt)
```c
static int xmp_write(const char *path, const char *buf, size_t size, ...)
{
    char enc_path[PATH_MAX];
    get_enc_path(enc_path, path);
    int fd = open(enc_path, O_WRONLY);
    
    // Encrypt: XOR data sebelum ditulis
    char *enc_buf = malloc(size);
    memcpy(enc_buf, buf, size);
    xor_cipher(enc_buf, size);
    
    res = pwrite(fd, enc_buf, size, offset);
    free(enc_buf);
    return res;
}
```
Saat user menyimpan atau mengedit sebuah file pada lingkungan FUSE, buffer string yang dimasukkan *user* (berupa text *plaintext*) akan disalin sementara ke *memory block* `enc_buf`, di XOR dengan kunci 0x76 hingga menjadi *ciphertext*, kemudian baru di-tulis (write) secara harfiah ke memori disk/harddisk di sisi original folder terenkripsinya.

###### Callback: xmp_read (Auto Decrypt)
```c
static int xmp_read(const char *path, char *buf, size_t size, off_t offset, ...)
{
    char enc_path[PATH_MAX];
    get_enc_path(enc_path, path);
    int fd = open(enc_path, O_RDONLY);

    res = pread(fd, buf, size, offset);
    if (res != -1)
    {
        // Decrypt: XOR data yang dibaca
        xor_cipher(buf, res);
    }
    return res;
}
```
Sebaliknya, pada operasi read, system call `pread` membaca deretan bytes dari file ciphertext (yang acak bentuknya), dan di-XOR kembali secara simetris hingga wujud string *plaintext* aslinya kembali tersusun di pointer return `buf` yang di-expect oleh user/caller.

##### client.c

Client `client.c` difungsikan untuk berbicara secara TCP Socket dengan DB Server di sisi Docker.

###### Loop & Send
```c
void send_and_receive(const char *cmd)
{
    char send_buf[MAX_BUFFER];
    snprintf(send_buf, sizeof(send_buf), "%s\n", cmd);
    send(sock_fd, send_buf, strlen(send_buf), 0);

    // Terima dan cetak respon dari server
    char recv_buf[MAX_BUFFER];
    int bytes;
    while ((bytes = recv(sock_fd, recv_buf, sizeof(recv_buf) - 1, 0)) > 0)
    {
        recv_buf[bytes] = '\0';
        printf("%s", recv_buf);
        if (bytes < (int)(sizeof(recv_buf) - 1)) break;
    }
}
```
Program membuat socket yang terhubung pada `127.0.0.1:9000`. Program berada pada input loop tanpa akhir menggunakan perintah standard *fgets*, setiap query yang disisipkan user dikirim dengan command socket `send`. Kemudian, thread utama akan *blocking* dan membaca respon lewat call `recv` (sampai *buffer length* kosong dan selesai merespons seluruh stream).

##### Dockerfile
```dockerfile
FROM ubuntu:latest
WORKDIR /app
COPY server /app/server
RUN mkdir -p /app/db
RUN chmod +x /app/server
EXPOSE 9000
CMD ["./server"]
```
Server binary sudah disiapkan dan di-*copy* langsung ke dalam file system container dengan base image `ubuntu:latest`. Kita mendeklarasikan tempat *mounting* atau working-dirnya menggunakan perintah `mkdir /app/db`. Eksekusi terminal untuk mengaktifkan DB Server langsung dilakukan begitu service docker dinyalakan.
#### Output
1. Melakukan Fuse 
![](Assets/Pasted%20image%2020260517222357.png)
2. Client terhubung
![](Assets/Pasted%20image%2020260517222653.png)
3. Membuat File
![](Assets/Pasted%20image%2020260517222836.png)
4. Enkripsi 
![](Assets/Pasted%20image%2020260517222857.png)
5. Container docker berjalan
![](Assets/Pasted%20image%2020260517222505.png)
#### Kendala
Tidak ada kendala.

---

### Soal 3: LibraryIT Server

Pada soal ini praktikan diminta untuk membuat arsitektur Samba File Server di ekosistem Docker dengan pengaturan peran Group dan User Authorization spesifik, dengan logging audit.

#### Penjelasan

Seluruh environment soal nomor 3 berjalan murni di atas *Docker* beserta Bash Provisioning di awal Container Bootup.

##### docker-compose.yml
```yaml
services:
  libraryit-server:
    build: .
    container_name: libraryit-server
    ports:
      - "1445:445"
      - "139:139"
    volumes:
      - ./data:/libraryit
      - ./logs:/var/log/samba-custom
    restart: unless-stopped
    
  libraryit-logger:
    ...
    command: sh -c "tail -f /var/log/samba-custom/libraryit.log"
    depends_on:
      - libraryit-server
```
Konfigurasi compose ini mendefinisikan main server file-sharing Samba di container `libraryit-server`. Port SMB/CIFS original `445` diexpose sebagai `1445` di host, dan SMB over NBT `139` di `139` ke Host. Host juga memount volume file asli lokal(`./data`) ke `/libraryit` untuk mempertahankan persistensi file. Container logger disertakan untuk mem-*stream* hasil log secara daemon ke container sebelahnya.

##### Dockerfile & entrypoint.sh
Di `Dockerfile`, paket dasar samba diinstall dengan package manager apt `apt-get install -y samba`.

Agar User dan konfigurasi terbentuk *on runtime*, docker entrypoint dijalankan di `entrypoint.sh`:
```bash
# Membuat Group (Readonly & Staff)
groupadd readonly
groupadd staff

# Menambahkan user
useradd -M -s /usr/sbin/nologin -G readonly member
useradd -M -s /usr/sbin/nologin -G staff contributor
useradd -M -s /usr/sbin/nologin -G staff librarian

# Inisiasi password SAMBA
(echo "member123"; echo "member123") | smbpasswd -s -a member
# ... lanjut untuk seluruh user...

# Mengatur Hak Akses (Syarat Folder)
mkdir -p /libraryit/ebooks /libraryit/papers /libraryit/sourcecode /libraryit/docs

# chown root dengan group staff
chown root:staff /libraryit/ebooks /libraryit/papers
chmod 775 /libraryit/ebooks /libraryit/papers

# untuk sourcecode hanya internal (tidak r-x oleh others)
chown root:staff /libraryit/sourcecode
chmod 750 /libraryit/sourcecode

exec smbd -F --no-process-group
```
Semua user tidak diberi akses ke Shell di sistem linux (flag `nologin`) karena murni digunakan untuk Samba authentication. Setelah membuat *UNIX Level Permissions*, service daemon Samba (`smbd`) dinyalakan di foreground untuk menahan Container agar tidak mati (exit state). 

##### smb.conf (Samba Rules Definition)

Di file `smb.conf` ini, *Role Based Access Control* (RBAC) pada *Virtual Share* dipisahkan secara gamblang.

```ini
[global]
    # ...
    # Konfigurasi dasar untuk audit logging
    vfs objects = full_audit
    full_audit:prefix = %u|%m|%S
    full_audit:success = mkdir rmdir read write rename
    full_audit:failure = connect
```
Sistem diinstruksikan lewat modul `vfs objects = full_audit` agar men-*tracking* semua tindakan modifikasi/pembacaan file (`mkdir`, `rmdir`, `read`, dsb.) ke sistem logging bawaan linux. 

```ini
[ebooks]
    path = /libraryit/ebooks
    valid users = @staff, @readonly
    write list = @staff

[sourcecode]
    path = /libraryit/sourcecode
    valid users = @staff
```
Di level resource path (`[ebooks]` & `[papers]`), group `@readonly` diatur sebagai *valid users* namun mereka tidak masuk list whitelist *write list*, sehingga role Member bisa membaca tapi tidak memodifikasi.

Pada bagian `[sourcecode]`, valid users dibatasi mutlak hanya pada list `@staff` saja. Sehingga Member (`@readonly`) di level otentikasi virtual directory Samba sudah akan terblokir sebelum membaca file UNIX itu sendiri.
#### Output
1. Container docker berjalan
![](Assets/Pasted%20image%2020260517223137.png)
2. Login sebagai member
![](Assets/Pasted%20image%2020260517223245.png)
3. Login sebagai contributor 
![](Assets/Pasted%20image%2020260517223401.png)
4. Permission member
- Dilarang mkdir hanya boleh ls
![](Assets/Pasted%20image%2020260517223311.png)
- Ditolak mengakses soucecode
![](Assets/Pasted%20image%2020260517223516.png)
5. Permission contributor
![](Assets/Pasted%20image%2020260517223442.png)
Dapat mkdir dan ls
6. cat libraryit.log
![](Assets/Pasted%20image%2020260517223613.png)
#### Kendala
Tidak ada kendala.

#### Revisi

Terdapat tiga masalah yang menyebabkan user tidak bisa melakukan login ke Samba server melalui `smbclient`. Penyebab utamanya adalah **paket `samba-vfs-modules` tidak terinstall**, **line ending CRLF pada `smb.conf`** yang merusak parsing nama group, dan **`rsyslog` tidak tersedia** sehingga modul `full_audit` tidak bisa menulis log.

##### Perubahan Kode

Pada `Dockerfile`, paket `samba-vfs-modules` ditambahkan agar modul `full_audit` yang dideklarasikan di `smb.conf` bisa dimuat oleh `smbd`. Tanpa modul ini, Samba akan menolak semua koneksi masuk. Paket `rsyslog` juga ditambahkan agar facility `local5` yang digunakan `full_audit` punya daemon syslog untuk menulis log:

```dockerfile
# Sebelum (hanya samba)
RUN apt-get update && apt-get install -y samba && rm -rf /var/lib/apt/lists/*

# Sesudah (ditambah vfs modules dan rsyslog)
RUN apt-get update && apt-get install -y samba samba-vfs-modules rsyslog && rm -rf /var/lib/apt/lists/*
```

File `smb.conf` dikonversi dari CRLF (Windows) ke LF (Unix). Masalah ini menyebabkan Samba mem-parsing nama group `readonly\r` alih-alih `readonly`, sehingga user `member` yang berada di group `readonly` tidak pernah cocok dan selalu ditolak aksesnya.

Pada `entrypoint.sh`, blok inisialisasi `rsyslog` ditambahkan sebelum `smbd` dijalankan. Konfigurasi ini merutekan semua pesan dari facility `local5` (yang digunakan `full_audit`) ke file `/var/log/samba-custom/libraryit.log`:

```bash
# 6. Setup log rsyslog untuk audit
mkdir -p /var/log/samba-custom
touch /var/log/samba-custom/libraryit.log
chown syslog:adm /var/log/samba-custom/libraryit.log

echo "local5.* /var/log/samba-custom/libraryit.log" > /etc/rsyslog.d/50-samba-audit.conf
service rsyslog start
```

File `Dockerfile.logger` juga ditambahkan karena `docker-compose.yml` mereferensikan file tersebut untuk service `libraryit-logger` namun file tersebut belum ada:

```dockerfile
FROM alpine:latest
RUN apk add --no-cache coreutils
```
