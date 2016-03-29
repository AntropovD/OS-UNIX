#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "lock.h"

void update_password_file(char *filename, char *username, char *password);

int main(int argc, char ** argv)
{
    if (argc != 4) {
        printf("Change username and password in passwd file with own file blocking system\n");
        printf("Usage: %s filename username new_password\n", argv[0]);
        return 1;
    }
    char *filename = argv[1];
    char *username = argv[2];
    char *password = argv[3];

    if (!is_file_exist(filename))
        return 1;

    update_password_file(filename, username, password);

    return 0;
}

void update_password_file(char *filename, char *username, char *password) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    long position = 0;
    printf("%ld", (long)getpid());
    while (1) {               
        int fd;
        lock(filename, &fd,  READ);
        FILE *fr = fdopen(fd, "r");
        fseek(fr, position, SEEK_SET);
        read = getline(&line, &len, fr);
        if (read == -1)
            break;
        printf("New read portion\n");
        //sleep(1);
        fclose(fr);
        unlock(filename, fd, READ);

        char *tmp_line = (char *)malloc(sizeof(char) * read);
        strncpy(tmp_line, line, read);
        char *old_username = strtok(line, " ");
        if (strcmp(username, old_username) == 0) {
            int tmp;
            lock(filename, &tmp, READ);
            FILE *rp = fopen(filename, "r");
            lock(".tmp", &tmp, WRITE);
            FILE *wp = fopen(".tmp", "w");
            int i, c;
            for (i = 0; i < position; i++) {
                c = fgetc(rp);
                fputc(c, wp);
            }
            fprintf(wp, "%s %s\n", username, password);
            fseek(rp, read, SEEK_CUR);
            while ((c=fgetc(rp)) != EOF) {
                fputc(c, wp);
            }

            fclose(rp);
            sleep(1);
            fclose(wp);
            sleep(1);
            unlock(filename, tmp, READ);

            lock(filename, &tmp, WRITE);
            remove(filename);
            sleep(1);
            rename(".tmp", filename);
            sleep(1);
            unlock(filename, tmp,  WRITE);
            sleep(1);
            unlock(".tmp", tmp, WRITE);
        }
        position += read;        
    }
    free(line);
}
