// Kenz Rescue — FUSE Passthrough + Virtual File
#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
/*
XoXoXoXoXoXoXoXo
Global Variables
XoXoXoXoXoXoXoXo
*/
static char source_dir[PATH_MAX];

static const char *virtual_file_name = "tujuan.txt";
static const char *virtual_file_path = "/tujuan.txt";
/*
XoXoXoXoXoXoXoXo
Helper: bangun full path dari source_dir + FUSE path
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
Helper: bangkitkan isi tujuan.txt on-the-fly
Scan KOORD: <...> dari 1.txt sampai 7.txt
XoXoXoXoXoXoXoXo
*/
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
                    char *nl = strchr(pos, '\n');
                    if (nl) *nl = '\0';
                    char *cr = strchr(pos, '\r');
                    if (cr) *cr = '\0';
                    strcat(combined, pos);
                    break;
                }
            }
            fclose(f);
        }
    }
    sprintf(buffer, "Tujuan Mas Amba: %s\n", combined);
}
/*
XoXoXoXoXoXoXoXo
Callback: getattr — stat file/directory
XoXoXoXoXoXoXoXo
*/
static int xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;

    // Virtual file: tujuan.txt
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

    // Passthrough ke source
    char full_path[PATH_MAX];
    get_full_path(full_path, path);

    res = lstat(full_path, stbuf);
    if (res == -1)
        return -errno;

    return 0;
}
/*
XoXoXoXoXoXoXoXo
Callback: readdir — list isi directory
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

        if (filler(buf, de->d_name, &st, 0))
            break;
    }

    closedir(dp);

    // Inject virtual file di root directory
    if (strcmp(path, "/") == 0)
    {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_mode = S_IFREG | 0444;
        filler(buf, virtual_file_name, &st, 0);
    }

    return 0;
}
/*
XoXoXoXoXoXoXoXo
Callback: open — buka file
XoXoXoXoXoXoXoXo
*/
static int xmp_open(const char *path, struct fuse_file_info *fi)
{
    // Virtual file: read-only saja
    if (strcmp(path, virtual_file_path) == 0)
    {
        if ((fi->flags & O_ACCMODE) != O_RDONLY)
            return -EACCES;
        return 0;
    }

    // Passthrough ke source
    int res;
    char full_path[PATH_MAX];
    get_full_path(full_path, path);

    res = open(full_path, fi->flags);
    if (res == -1)
        return -errno;

    close(res);
    return 0;
}
/*
XoXoXoXoXoXoXoXo
Callback: read — baca isi file
XoXoXoXoXoXoXoXo
*/
static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi)
{
    // Virtual file: bangkitkan isi on-the-fly
    if (strcmp(path, virtual_file_path) == 0)
    {
        char content_buffer[2048];
        get_tujuan_content(content_buffer);
        size_t len = strlen(content_buffer);
        if ((size_t)offset < len)
        {
            if (offset + size > len)
                size = len - offset;
            memcpy(buf, content_buffer + offset, size);
        }
        else
        {
            size = 0;
        }
        return size;
    }

    // Passthrough ke source
    int fd;
    int res;
    char full_path[PATH_MAX];
    get_full_path(full_path, path);

    (void) fi;
    fd = open(full_path, O_RDONLY);
    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);
    return res;
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
    .open       = xmp_open,
    .read       = xmp_read,
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
        fprintf(stderr, "Usage: %s <source_directory> <mount_directory>\n", argv[0]);
        return 1;
    }

    /*
    _o_o_o_o_o_o_o
    Resolve absolute path dari source_directory
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
    Modifikasi argv agar fuse_main hanya melihat mount_directory
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