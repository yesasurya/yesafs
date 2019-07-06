#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

char ABSOLUTE_PATH[PATH_MAX];

static char* concat(const char *s1, const char *s2) {
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

static int do_getattr(const char *path, struct stat *st) {
    st->st_uid = getuid();
	st->st_gid = getgid();
	st->st_atime = time( NULL );
	st->st_mtime = time( NULL );
    if (strcmp(path, "/") == 0) {
		st->st_mode = S_IFDIR | S_IRWXU;
		st->st_nlink = 2;
	} else {
		st->st_mode = S_IFREG | S_IRWXU;
		st->st_nlink = 1;
	}

	return 0;
}

static int do_truncate(const char *path, off_t offset) {
    return truncate(concat(ABSOLUTE_PATH, path), offset);
}

static int do_open(const char *path, struct fuse_file_info *fi) {
    fi->fh = open(concat(ABSOLUTE_PATH, path), O_CREAT | O_RDWR, S_IRWXU);
    return 0;
}

static int do_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    return pread(fi->fh, buf, size, offset);
}

static int do_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    return pwrite(fi->fh, buf, size, offset);
}

static int do_flush(const char *path, struct fuse_file_info *fi) {
    return fsync(fi->fh);
}

static int do_release(const char *path, struct fuse_file_info *fi) {
    return close(fi->fh);
}

static int do_readdir(const char *path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    return 0;
}

static struct fuse_operations operations = {
    .getattr    = do_getattr,
    .truncate   = do_truncate,
    .open       = do_open,
    .write      = do_write,
    .read       = do_read,
    .flush      = do_flush,
    .release    = do_release,
    .readdir    = do_readdir
};

int main(int argc, char *argv[]) {
    getcwd(ABSOLUTE_PATH, sizeof(ABSOLUTE_PATH));
    return fuse_main(argc, argv, &operations, NULL);
}
