#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#define BUF_SIZE 1024


int main(int argc, char ** argv) {

    if (argc != 3) {
        printf("Gzip addition for sparced files\n");
        printf("Usage: %s inputFile outputFile\n", argv[0]);
        return 1;
    }

    int input = open(argv[1], O_RDONLY);
    if (input == -1) {
        printf("Cannot open file.\n");
        return 2;
    }

    int output = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output == -1) {
        printf("Cannot open file.\n");
        return 2;
    }
    // Изначально создаем пустой sparce file нужного размера, необходимо
    // это так как после lseek надо делать write какой то мусор, иначе на
    // диск не будет записи.
    struct stat st;
    fstat(input, &st);
    ftruncate(output, st.st_size);

    int n, i;
    off_t count = 0;
    char buff[BUF_SIZE];
    while((n = read(input, buff, BUF_SIZE)) > 0) {
        char *ptr = buff;
        for (i = 0; i < n; i++) {
            if (buff[i] == 0)
                count++;
            else {
                if (count != 0) {
                    lseek(output, count, SEEK_CUR);
                    count = 0;
                }
                write(output, ptr, 1);
            }
            ptr++;
        }
        if (count != 0){
            lseek(output, count, SEEK_CUR);
            count = 0;
        }
    }
    close(input);
    close(output);
    return 0;
}
