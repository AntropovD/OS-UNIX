#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "lock.h"

char *can_handle_next_line(char *filename, long int *position, ssize_t *read);

void modify_user_and_password_in_file(char *filename, char *username,
                                      char *password, long int position, ssize_t read);

void update_password_file(char *filename, char *username, char *password);

int main(int argc, char ** argv)
{
    if (argc != 4) {
        printf("Change username and password in passwd file with own \
               file blocking system\n");
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
    long position = 0;
    ssize_t read;
    if (DEBUG)
        printf("DEBUG: Pid process %ld\n", (long)getpid());

    while (1) {               
        line = can_handle_next_line(filename, &position, &read);
        if (line == NULL)
            break;

        char *tmp_line = (char *)malloc(sizeof(char) * read);
        strncpy(tmp_line, line, read);
        char *old_username = strtok(line, " ");
        if (strcmp(username, old_username) == 0) {
            modify_user_and_password_in_file(filename, username,
                                             password, position, read);
        }
        if (DEBUG)
            printf("###############################################\n");
        free(line);
        free(tmp_line);
        position += read;        
    }
    free(line);
}

char *can_handle_next_line(char *filename, long int *position, ssize_t *read)
{
    char *line = NULL;
    int fd;
    size_t len = 0;
    lock(filename, &fd,  READ);
    FILE *fr = fdopen(fd, "r");
    fseek(fr, *position, SEEK_SET);
    *read = getline(&line, &len, fr);
    if (*read == -1) {
        fclose(fr);
        unlock(filename, fd, READ);
        return NULL;
    }
    if (DEBUG)
        printf("DEBUG: +\n");
    if (SLEEP) sleep(1);
    fclose(fr);
    unlock(filename, fd, READ);
    return line;
}

void modify_user_and_password_in_file(char *filename, char *username,
                                      char *password, long int position, ssize_t read)
{
    if (DEBUG)
        printf("DEBUG: Start modifiyng\n");
    int tmp;
    lock(filename, &tmp, WRITE);
    lock(".tmp", &tmp, WRITE);
    FILE *rp = fopen(filename, "r");
    FILE *wp = fopen(".tmp", "w");
    int i, c;
    for (i = 0; i < position; i++) {
        c = fgetc(rp);
        fputc(c, wp);
    }
    if (SLEEP) sleep(3);
    fprintf(wp, "%s %s\n", username, password);
    if (SLEEP) sleep(3);
    fseek(rp, (long)read, SEEK_CUR);
    while ((c=fgetc(rp)) != EOF) {
        fputc(c, wp);
    }
    if (SLEEP) sleep(3);
    fclose(rp);
    fclose(wp);

    if (DEBUG)
        printf("DEBUG: Rewrite starts\n");
    if (SLEEP) sleep(3);
    remove(filename);
    rename(".tmp", filename);
    unlock(filename, tmp,  WRITE);
    unlock(".tmp", tmp, WRITE);
    if (DEBUG)
        printf("DEBUG: Finish modifiyng\n");
}
