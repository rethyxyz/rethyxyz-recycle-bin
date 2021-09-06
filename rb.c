#include <string.h>
#include <libgen.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

/*
 * TODOs
 *
 * - Get number of files in directory.
 *
*/

// Init functions.
int empty_dir(char *path);
int empty_file(const char *path);
int is_dir(const char *directory);
int is_file(const char *path);
int mv_file(char *src, char *trash_bin);
int mv_dir(char *dir, char *trash_bin);
int rm(char *path);
void help(char *filename);

// This is sort of convoluted. I'll have to revise this when I get better time.
int mv_dir(char *dir, char *trash_bin) {
    // This is used later on to skip the first two files, being "." and ".." .
    // meaning this dir, and parent dir.
    int counter = 0, incrementor = 0;

    char trash_bin_src[255];
    char trash_bin_dest[255];

    // Pointer for directory entry
    struct dirent *de;

    // directory pointer.
    DIR *dir_ptr = opendir(dir);

    strcpy(trash_bin_dest, trash_bin);
    strcat(trash_bin_dest, "/");
    strcat(trash_bin_dest, dir);

    while (1) {
        if (is_file(trash_bin_dest) == 0) {
            strcpy(trash_bin_dest, trash_bin);
            strcat(trash_bin_dest, "/");
            strcat(trash_bin_dest, incrementor + "0");
            strcat(trash_bin_dest, "_");
            strcat(trash_bin_dest, dir);
        } else {
            printf("Name is \"%s\"\n", trash_bin_dest);
            mkdir(trash_bin_dest, 0777);
            break;
        }

        incrementor++;
        printf("Currently at %d.\nRepeating. \n", incrementor);
    }


    if (dir_ptr == NULL) {
        printf("Could not open current directory" );
        return 1;
    }

    while ((de = readdir(dir_ptr)) != NULL) {
        printf("%s\n", de->d_name);

        if (counter >= 2) {
            printf("Greater than or equal to 2!\n");

            // Build the src file from source-directory/sub-file.
            strcpy(trash_bin_src, dir);
            strcat(trash_bin_src, "/");
            strcat(trash_bin_src, de->d_name);

            printf("mv_file(%s, %s);\n", trash_bin_src, trash_bin);
            mv_file(trash_bin_src, trash_bin_dest);
        }

        counter++;
    }

    closedir(dir_ptr);

    remove(dir);

    return 0;
}

int rm(char *path) {
    if (remove(path) == 0) {
        printf("|Status: Removed\n\n");
        return 0;
    }

    printf("|Status: Remove failed\n\n");
    return 1;
}

int mv_file(char *src, char *dest) {
    char ch;

    FILE *f_ptr = fopen(src, "r");
    FILE *f_ptr_2 = fopen(dest, "w");

    if (! f_ptr) {
        printf("Couldn't open \"%s\" for reading.\n", src);
        return 1;
    }

    if (! f_ptr_2) { printf("Couldn't open \"%s\" for writing.\n", dest);
        return 1;
    }

    while ((ch = fgetc(f_ptr)) != EOF) {
        fputc(ch, f_ptr_2);
    }

    fclose(f_ptr);
    fclose(f_ptr_2);

    printf("Sucessfully wrote to file \"%s\"\n", dest);

    remove(src);
    return 0;
}

int empty_dir(char *path) {
    int n = 0;
    struct dirent *d;
    DIR *dir = opendir(path);

    //Not a directory or doesn't exist
    if (dir == NULL) {
        printf("Here 3\n");
        return -1;
    }

    while ((d = readdir(dir)) != NULL) {
        if (++n > 2) {
            break;
        }
    }

    closedir(dir);

    //Directory Empty
    if (n <= 2) {
        return 0;
    } else {
        return 1;
    }
}

int is_file(const char *path) {
    // Try to open path
    FILE *f_ptr = fopen(path, "r");

    // If path does not exists
    if (f_ptr == NULL) {
        return 1;
    }

    // path exists hence close path and return true.
    fclose(f_ptr);

    return 0;
}

int empty_file(const char *path) {
    FILE *f_ptr = fopen(path, "r");

    if (f_ptr != NULL) {
        fseek(f_ptr, 0, SEEK_END);
        double size = ftell(f_ptr);

        if (size == 0) {
            return 0;
        }
    }

    return 1;
}

int is_dir(const char *directory) {
    DIR* dir = opendir(directory);

    if (dir) {
        closedir(dir);
        return 0;
    } else if (ENOENT == errno) {
        return 1;
    }

    return 1;

    // I'll make 2 its own thing in the future, meaning something extra or
    // so.
    //
    // return 2;
}

void help(char *filename) {
    printf("\n%s [ITEM] [ITEM 2] [ITEM 3] ...\n", filename);
    printf("List the ITEM(s) you want to remove.\n");
}

int main(int argc, char *argv[]) {
    /*++++++++++++++++++++++*/
    /* Variable Definitions */
    /*++++++++++++++++++++++*/

    // Ensure there's a NULL terminator at the end of the variable. If not,
    // it's going to spit out some weird shit.
    char trash_bin[255] = "/home/brody/.trash\0";
    char *dest_final[255];
    int unsigned i;

    // for (i = 0; i < sizeof(trash_bin); i++) {
    //     printf("%c", trash_bin[i]);
    // }

    // If no args given, printf and quit.
    if (argc < 2) {
        printf("No filename(s) given.\n\n");
        help(argv[0]);
        return 1;
    }

    // Else if an arg matches, display help function.
    for (i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            help(argv[0]);
            return 0;
        }
    }

    // If trash_bin directory doesn't exist, create it.
    if (is_dir(trash_bin) != 0) {
        if (mkdir(trash_bin, 0777) != 0) {
            printf("Couldn't make directory %s.\n", trash_bin);
            return 1;
        }

        printf("Made directory %s\n", trash_bin);
    }

    for (i = 1; i < argc; i++) {
        // Check if exists at all. Is file works for this.
        if (is_file(argv[i]) != 0) {
            printf("Item \"%s\" doesn't exist.\n\n", argv[i]);
            continue;
        }

        printf("|Name: %s\n", argv[i]);

        // Check if dir exists.
        if (is_dir(argv[i]) == 0) {
            printf("|Type: Directory\n");
            // Check if dir is empty.

            if (empty_dir(argv[i]) == 0) {
                printf("|Attribute: Empty\n");

                rm(argv[i]);
                continue;
            }

            mv_dir(argv[i], trash_bin);
        } else {
            printf("|Type: File\n");
            //if (is_link(argv[i]) == 0) {
            //    printf("File is a link\n");

            if (empty_file(argv[i]) == 0) {
                printf("|Attribute: Empty\n");

                rm(argv[i]);
                continue;
            }

            strcpy(dest_final, trash_bin);
            strcat(dest_final, "/");
            strcat(dest_final, argv[i]);

            mv_file(argv[i], dest_final);
        }
    }

    return 0;
}
