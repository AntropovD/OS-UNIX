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

    if (!check_file_exists_and_not_locked(filename))
        return 1;

    update_password_file(filename, username, password);

    return 0;
}

void update_password_file(char *filename, char *username, char *password) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    long position = 0;

    while (1) {
     //  lock(1);
        FILE *fp = fopen(filename, "r");
        fseek(fp, position, SEEK_SET);
        read = getline(&line, &len, fp);
        fclose(fp);
       // unlock(1);

        char *tmp_line = (char *)malloc(sizeof(char) * read);
        strncpy(tmp_line, line, read);
        char *old_username = strtok(line, " ");
        if (strcmp(username, old_username) == 0) {
//            lock(1);
//            FILE *rp = fopen(filename, "r");
//            lock(2);
//            FILE *wp = fopen(filename, "w");

            printf("%ld", position);
//            int i, c;

//            for (i = 0; i < position; i++) {
//                c = fgetc(rp);
//                fputc(c, wp);
//            }
//            for (i = 0; i < read; i++) {
//                fputc(line[i], wp);
//            }
//            fseek(wp, read, SEEK_CUR);
//            while ((c=fgetc(rp)) != EOF) {
//                fputc(c, wp);
//            }
//            close(rp);
//            close(fp);
        }

        position += read;

        if (read == -1)
            break;
    }
    free(line);
}

