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

// Path folder asli yang akan dibaca oleh FUSE
static const char *source_dir = "/home/arkano/Documents";

// Fungsi untuk memetakan path dari FUSE kembali ke path asli
void resolve_path(char *fpath, const char *path) {
    char temp[1000];
    strcpy(temp, path);

    // Mengecek apakah path berada di dalam direktori TOPSECRET_
    if (strstr(temp, "/TOPSECRET_") != NULL) {
        // Cari posisi slash terakhir untuk memisahkan nama file
        char *last_slash = strrchr(temp, '/');
        if (last_slash != NULL) {
            char filename[500];
            strcpy(filename, last_slash + 1); // Ambil nama file

            // Jika nama file berawalan [ENCRYPTED]_, hapus awalan tersebut 
            // agar bisa menunjuk ke file asli di storage
            if (strncmp(filename, "[ENCRYPTED]_", 12) == 0) {
                *last_slash = '\0'; // Potong string temp sampai batas direktori
                sprintf(fpath, "%s%s/%s", source_dir, temp, filename + 12);
                return;
            }
        }
    }
    
    // Default: jika bukan file terenkripsi
    if (strcmp(path, "/") == 0) {
        sprintf(fpath, "%s", source_dir);
    } else {
        sprintf(fpath, "%s%s", source_dir, path);
    }
}

static int xmp_getattr(const char *path, struct stat *stbuf) {
    char fpath[1000];
    resolve_path(fpath, path);
    
    int res = lstat(fpath, stbuf);
    if (res == -1) return -errno;
    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    char fpath[1000];
    resolve_path(fpath, path);

    DIR *dp = opendir(fpath);
    if (dp == NULL) return -errno;

    struct dirent *de;
    // Cek apakah direktori saat ini mengandung kata TOPSECRET_
    int is_secret = (strstr(fpath, "/TOPSECRET_") != NULL);

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        char display_name[500];
        strcpy(display_name, de->d_name);

        // Manipulasi nama file hanya untuk file biasa (bukan direktori)
        if (is_secret && de->d_type != DT_DIR) {
            sprintf(display_name, "[ENCRYPTED]_%s", de->d_name);
        }

        if (filler(buf, display_name, &st, 0)) break;
    }
    closedir(dp);
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    char fpath[1000];
    resolve_path(fpath, path);

    // LOGGING
    FILE *log_file = fopen("/home/arkano/access.log", "a");
    if (log_file != NULL) {
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        
        // Ekstrak nama file saja dari path FUSE
        const char *filename = strrchr(path, '/');
        if (filename != NULL) filename++; else filename = path;

        fprintf(log_file, "[WARNING] Akses Data: %s pada %02d-%02d-%04d %02d:%02d:%02d\n", 
                filename, tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900,
                tm->tm_hour, tm->tm_min, tm->tm_sec);
        fclose(log_file);
    }

    int fd = open(fpath, O_RDONLY);
    if (fd == -1) return -errno;

    int res = pread(fd, buf, size, offset);
    if (res == -1) res = -errno;
    close(fd);
    return res;
}

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read    = xmp_read,
};

int main(int argc, char *argv[]) {
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}

