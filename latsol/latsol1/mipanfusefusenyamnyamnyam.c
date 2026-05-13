#define FUSE_USE_VERSION 31
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

char source_dir[6767];

void pathCombine(char *fpath, const char *path) 
{
    char tempPath[6767];
    strcpy(tempPath, path);
    // Cek apakah user sedang mencoba mengakses file di dalam TOPSECRET_
    if (strstr(tempPath, "/TOPSECRET_") != NULL) 
    {
        char *last_slash = strrchr(tempPath, '/');
        if (last_slash != NULL) {
            char *filename = last_slash + 1;
            // Jika file yang diakses diawali [ENCRYPTED]_, potong stringnya!
            if (strncmp(filename, "[ENCRYPTED]_", 12) == 0) {
                memmove(filename, filename + 12, strlen(filename + 12) + 1);
            }
        }
    }

    // Gabungkan source_dir dengan path asli yang sudah dibersihkan
    if (strcmp(tempPath, "/") == 0) {
        sprintf(fpath, "%s", source_dir);
    } else {
        sprintf(fpath, "%s%s", source_dir, tempPath);
    }
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    
    // dapatkan lokasi aslinya di Documents
    char fpath[6767];
    pathCombine(fpath, path);

    // buka direktori asli (membaca isi harddisk sesungguhnya)
    DIR *dp = opendir(fpath);
    struct dirent *de;

    // cek apakah kita sedang berada di dalam folder rahasia
    int isTopSecret = (strstr(path, "/TOPSECRET_") != NULL);

    // baca isi folder asli satu per satu
    while ((de = readdir(dp)) != NULL) 
    {
        char nama_tampil[6767];
        // Salin nama asli (misal: "laporan.pdf")
        strcpy(nama_tampil, de->d_name);

        // tempelkan prefix [ENCRYPTED]_ di depannya.
        if (isTopSecret && strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) 
        {
            sprintf(nama_tampil, "[ENCRYPTED]_%s", de->d_name);
        }

        filler(buf, nama_tampil, NULL, 0); // menampilkan nama file ke layar terminal
    }
    
    closedir(dp);
    return 0;
}

static struct fuse_operations xmp_oper = 
{
    .getattr    = xmp_getattr, // get file attr (.exe etc)
    .readdir    = xmp_readdir, // read the inside of a dir
    .read       = xmp_read, // read file
};

char userDir[1024];
int main(int argc, char *argv[]) 
{
    // 2. Set alamat sumber secara dinamis
    sprintf(userDir, "%s/Documents", getenv("HOME"));
    return fuse_main(argc, argv, &xmp_oper, NULL);
}
