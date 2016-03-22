#include <stdio.h>
#include <sys/stat.h>

int doesFileExist(const char *filename);

int main(int argc, char ** argv) {
    if (argc != 2) {
        printf("File blocking program\n");
        printf("Usage: %s filename\n", argv[0]);
        return 1;
    }




    return 0;
}

int doesFileExist(const char *filename) {
        struct stat st;
            int result = stat(filename, &st);
                return result == 0;
}
