#define FUSE_USE_VERSION 30

#include "fuse.h"
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>


//
// Function getAttributes: Gets called when system asks our FS attributes of a file

    // path: Path of the FILE whose attributes are required
    // st: Attributes of the FILE should be filled into this "stat" structure

struct stat {
    int st_uid;
    int st_gid;
    time_t st_atime;
    time_t st_mtime;
    size_t st_mode;
    int st_nlink;
    int st_size;
};

static int getAttributes (const char *path, struct stat *st) {

    // GNU's definitions of Attributes: http://www.gnu.org/software/libc/manual/html_node/Attribute-Meanings.html

    printf("Getting the Attributes of file %s \n", path);
    st -> st_uid = getpid();        // User ID: Owner of the FILE (user who mounted the file system)
    // st -> st_gid = getgid();        // Group ID: Owner Group of the FILE
    st -> st_atime = time(NULL);    // Last Access Time (in UNIX Time: seconds passed since 00:00:00 UTC, January 1, 1970)
    st -> st_mtime = time(NULL);    // Last Modification Time

    // If file is the ROOT directory
    if(strcmp(path, "/") == 0) {
        st -> st_mode = S_IFDIR | 0755;     // Permission Bits (user: rwx, group:r-x, other:r-x)
        st -> st_nlink = 2;                 // No of Hardlinks (/. and /.. by default + (n = no of subdirectories) = 2)
        // S_IFDIR is a macro
    }

    // If file is a REGULAR file
    else {
        st -> st_mode = S_IFREG | 0644;
        st -> st_nlink = 1;
        st -> st_size = 1024;               // Size of File
    }

    return 0;
}


// ----------------------
// Function listDirectory: Lists the files inside a specified directory
    // path: Path of the DIRECTORY
    // buffer: Fill the names of files in the DIRECTORY
    // filler: Sent by FUSE, used to fill "buffer"
        // typedef int (*fuse_fill_dir_t) (void *buf, const char *name, const struct stat *stbuf, off_t off);

static int (*listDirectory) (const char* path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    printf("Getting the list of files of %s \n", path);

    // buf: Pointer to buffer where we want to write the entry
    // name: Name of the current entry
    // stbuf: Pointer to structure containing info about directory entry (file attributes)

    filler(buffer, ".", NULL, 0);
    filler(buffer, "..", NULL, 0);

    if(strcmp(path, "/") == 0) {
        filler(buffer, "file54", NULL, 0);
        filler(buffer, "file349", NULL, 0);
    }
    return 0;
}


// ----------------------
// Function readFile: Read contents of the file

static int (*readFile) (const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {

    char file54Text[] = "Hello World From File54!";
	char file349Text[] = "Hello World From File349!";
	char *selectedText = NULL;

    if ( strcmp( path, "/file54" ) == 0 )
		selectedText = file54Text;
	else if ( strcmp( path, "/file349" ) == 0 )
		selectedText = file349Text;
	else
		return -1;

    // Copy "size" bytes from source "selectedText" to dest "buffer"
    memcpy(buffer, selectedText + offset, size);

    return strlen(selectedText) - offset;   
}


// -----------------------------------
// Fill the "fuse_operations" and tell FUSE about it

static struct fuse_operations operations = {
    .getattr = getAttributes,
    .readdir = listDirectory,
    .read = readFile,
};


int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &operations, NULL);
}