// Project MOO — FUSE Encrypted Passthrough
#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <limits.h>
/*
XoXoXoXoXoXoXoXo
Global Variables
XoXoXoXoXoXoXoXo
*/
static char source_dir[PATH_MAX]; // path absolut ke encrypted_storage
#define XOR_KEY 0x76
/*
XoXoXoXoXoXoXoXo
Helper: bangun full path di encrypted_storage
Tambahkan .enc di akhir jika path menunjuk file
XoXoXoXoXoXoXoXo
*/
static void get_full_path(char *full_path, const char *path)
{
    if (strcmp(path, "/") == 0)
    {
        sprintf(full_path, "%s", source_dir);
    }
    else
    {
        sprintf(full_path, "%s%s", source_dir, path);
    }
}

/*
XoXoXoXoXoXoXoXo
Helper: bangun full path dengan ekstensi .enc
Untuk file biasa, nama di encrypted_storage
selalu berakhiran .enc
XoXoXoXoXoXoXoXo
*/
static void get_enc_path(char *enc_path, const char *path)
{
    sprintf(enc_path, "%s%s.enc", source_dir, path);
}

/*
XoXoXoXoXoXoXoXo
Helper: XOR encrypt/decrypt buffer
Karena XOR bersifat simetris, fungsi ini
dipakai untuk encrypt maupun decrypt
XoXoXoXoXoXoXoXo
*/
static void xor_cipher(char *data, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        data[i] ^= XOR_KEY;
    }
}

/*
XoXoXoXoXoXoXoXo
Callback: getattr — stat file/directory
Memetakan path fuse_mount ke encrypted_storage
XoXoXoXoXoXoXoXo
*/
static int xmp_getattr(const char *path, struct stat *stbuf)
{
    char full_path[PATH_MAX];

    // Cek apakah ini directory
    get_full_path(full_path, path);
    int res = lstat(full_path, stbuf);

    if (res == 0 && S_ISDIR(stbuf->st_mode))
        return 0;

    // Kalau bukan directory, coba cari versi .enc
    if (strcmp(path, "/") != 0)
    {
        char enc_path[PATH_MAX];
        get_enc_path(enc_path, path);
        res = lstat(enc_path, stbuf);
        if (res == 0)
        {
            // Size yang ditampilkan = size asli (sama karena XOR)
            return 0;
        }
    }

    // Fallback: cek path biasa (mungkin directory di root)
    get_full_path(full_path, path);
    res = lstat(full_path, stbuf);
    if (res == -1)
        return -errno;

    return 0;
}

/*
XoXoXoXoXoXoXoXo
Callback: readdir — list isi directory
Menampilkan file tanpa ekstensi .enc
XoXoXoXoXoXoXoXo
*/
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi)
{
    char full_path[PATH_MAX];
    get_full_path(full_path, path);

    DIR *dp;
    struct dirent *de;

    (void) offset;
    (void) fi;

    dp = opendir(full_path);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL)
    {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        char display_name[PATH_MAX];
        strcpy(display_name, de->d_name);

        // Hilangkan ekstensi .enc dari nama yang ditampilkan
        size_t len = strlen(display_name);
        if (len > 4 && strcmp(display_name + len - 4, ".enc") == 0)
        {
            display_name[len - 4] = '\0';
        }

        if (filler(buf, display_name, &st, 0))
            break;
    }

    closedir(dp);
    return 0;
}

/*
XoXoXoXoXoXoXoXo
Callback: mkdir — buat directory baru
XoXoXoXoXoXoXoXo
*/
static int xmp_mkdir(const char *path, mode_t mode)
{
    char full_path[PATH_MAX];
    get_full_path(full_path, path);

    int res = mkdir(full_path, mode);
    if (res == -1)
        return -errno;

    return 0;
}

/*
XoXoXoXoXoXoXoXo
Callback: rmdir — hapus directory
XoXoXoXoXoXoXoXo
*/
static int xmp_rmdir(const char *path)
{
    char full_path[PATH_MAX];
    get_full_path(full_path, path);

    int res = rmdir(full_path);
    if (res == -1)
        return -errno;

    return 0;
}

/*
XoXoXoXoXoXoXoXo
Callback: create — buat file baru (dengan .enc)
XoXoXoXoXoXoXoXo
*/
static int xmp_create(const char *path, mode_t mode,
                      struct fuse_file_info *fi)
{
    char enc_path[PATH_MAX];
    get_enc_path(enc_path, path);

    int fd = open(enc_path, fi->flags, mode);
    if (fd == -1)
        return -errno;

    fi->fh = fd;
    return 0;
}

/*
XoXoXoXoXoXoXoXo
Callback: open — buka file (.enc di storage)
XoXoXoXoXoXoXoXo
*/
static int xmp_open(const char *path, struct fuse_file_info *fi)
{
    char enc_path[PATH_MAX];
    get_enc_path(enc_path, path);

    int fd = open(enc_path, fi->flags);
    if (fd == -1)
        return -errno;

    fi->fh = fd;
    return 0;
}

/*
XoXoXoXoXoXoXoXo
Callback: read — baca file, decrypt dengan XOR
Data di encrypted_storage terenkripsi,
jadi kita XOR lagi supaya bisa dibaca plaintext
XoXoXoXoXoXoXoXo
*/
static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi)
{
    int fd;
    int res;

    (void) fi;

    char enc_path[PATH_MAX];
    get_enc_path(enc_path, path);

    fd = open(enc_path, O_RDONLY);
    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);
    if (res == -1)
    {
        res = -errno;
    }
    else
    {
        // Decrypt: XOR data yang dibaca
        xor_cipher(buf, res);
    }

    close(fd);
    return res;
}

/*
XoXoXoXoXoXoXoXo
Callback: write — tulis file, encrypt dengan XOR
Data yang ditulis user di-XOR dulu
sebelum disimpan ke encrypted_storage
XoXoXoXoXoXoXoXo
*/
static int xmp_write(const char *path, const char *buf, size_t size,
                     off_t offset, struct fuse_file_info *fi)
{
    int fd;
    int res;

    (void) fi;

    char enc_path[PATH_MAX];
    get_enc_path(enc_path, path);

    fd = open(enc_path, O_WRONLY);
    if (fd == -1)
        return -errno;

    // Encrypt: XOR data sebelum ditulis
    char *enc_buf = malloc(size);
    if (!enc_buf)
    {
        close(fd);
        return -ENOMEM;
    }
    memcpy(enc_buf, buf, size);
    xor_cipher(enc_buf, size);

    res = pwrite(fd, enc_buf, size, offset);
    if (res == -1)
        res = -errno;

    free(enc_buf);
    close(fd);
    return res;
}

/*
XoXoXoXoXoXoXoXo
Callback: truncate — potong/set ukuran file
XoXoXoXoXoXoXoXo
*/
static int xmp_truncate(const char *path, off_t size)
{
    char enc_path[PATH_MAX];
    get_enc_path(enc_path, path);

    int res = truncate(enc_path, size);
    if (res == -1)
        return -errno;

    return 0;
}

/*
XoXoXoXoXoXoXoXo
Callback: unlink — hapus file (.enc di storage)
XoXoXoXoXoXoXoXo
*/
static int xmp_unlink(const char *path)
{
    char enc_path[PATH_MAX];
    get_enc_path(enc_path, path);

    int res = unlink(enc_path);
    if (res == -1)
        return -errno;

    return 0;
}

/*
XoXoXoXoXoXoXoXo
Callback: access — cek permission file/directory
XoXoXoXoXoXoXoXo
*/
static int xmp_access(const char *path, int mask)
{
    // Cek directory dulu
    char full_path[PATH_MAX];
    get_full_path(full_path, path);
    int res = access(full_path, mask);
    if (res == 0)
        return 0;

    // Cek file .enc
    if (strcmp(path, "/") != 0)
    {
        char enc_path[PATH_MAX];
        get_enc_path(enc_path, path);
        res = access(enc_path, mask);
        if (res == 0)
            return 0;
    }

    return -errno;
}

/*
XoXoXoXoXoXoXoXo
Callback: utimens — set access/modification time
XoXoXoXoXoXoXoXo
*/
static int xmp_utimens(const char *path, const struct timespec ts[2])
{
    // Cek directory dulu
    char full_path[PATH_MAX];
    get_full_path(full_path, path);

    struct stat st;
    if (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode))
    {
        int res = utimensat(AT_FDCWD, full_path, ts, AT_SYMLINK_NOFOLLOW);
        if (res == -1)
            return -errno;
        return 0;
    }

    // File: pakai path .enc
    char enc_path[PATH_MAX];
    get_enc_path(enc_path, path);

    int res = utimensat(AT_FDCWD, enc_path, ts, AT_SYMLINK_NOFOLLOW);
    if (res == -1)
        return -errno;

    return 0;
}

/*
XoXoXoXoXoXoXoXo
FUSE Operations Table
XoXoXoXoXoXoXoXo
*/
static struct fuse_operations xmp_oper =
{
    .getattr    = xmp_getattr,
    .readdir    = xmp_readdir,
    .mkdir      = xmp_mkdir,
    .rmdir      = xmp_rmdir,
    .create     = xmp_create,
    .open       = xmp_open,
    .read       = xmp_read,
    .write      = xmp_write,
    .truncate   = xmp_truncate,
    .unlink     = xmp_unlink,
    .access     = xmp_access,
    .utimens    = xmp_utimens,
};

int main(int argc, char *argv[])
{
    /*
    _o_o_o_o_o_o_o
    Validasi argumen
    _o_o_o_o_o_o_o
    */
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <encrypted_storage> <fuse_mount>\n", argv[0]);
        return 1;
    }

    /*
    _o_o_o_o_o_o_o
    Resolve absolute path dari encrypted_storage
    (FUSE akan chdir ke "/" saat jadi daemon)
    _o_o_o_o_o_o_o
    */
    if (realpath(argv[1], source_dir) == NULL)
    {
        perror("realpath");
        return 1;
    }

    /*
    _o_o_o_o_o_o_o
    Modifikasi argv agar fuse_main
    hanya melihat mount_directory
    _o_o_o_o_o_o_o
    */
    for (int i = 1; i < argc - 1; i++)
    {
        argv[i] = argv[i + 1];
    }
    argv[argc - 1] = NULL;
    argc--;

    return fuse_main(argc, argv, &xmp_oper, NULL);
}
