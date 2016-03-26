#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "lock.h"

#define BUFFER_SIZE 40
#define LINE_SIZE 40

char *get_next_line_from_buf(char *buf, int *i, int n);
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

int line_max_size = LINE_SIZE;
int count = 0;
char *line = NULL;

char *get_next_line_from_buf(char *buf, int *i, int n) {
    int j;
    for (j = *i; j < n; j++) {
         if (buf[j] == '\n') {
             char *lineult = malloc(sizeof(char) * (count + 1));
             memset(lineult, 0, count + 1);
             strncpy(lineult, line, count);
             count = 0;
             *i += j+1;
             return lineult;
         }
         if (count == line_max_size) {
             line_max_size += LINE_SIZE;
             line = realloc(line, line_max_size);
         }
         line[count] = buf[j];
         count++;
    }
    *i += j;
    return NULL;
}

void update_password_file(char *filename, char *username, char *password) {
    bool over = false;
    char *buf = (char *)malloc(sizeof(char) * line_max_size);
    int n, fd;
    off_t position = 0;

    line = (char *)malloc(sizeof(char) * line_max_size);

    while (!over) {
       fd = open(filename, O_RDONLY);
       lock(fd);
       lseek(fd, position, SEEK_CUR);
       n = read(fd, buf, BUFFER_SIZE);
       position += n;

       char *line;
       int i = 0;
       while (1) {
           line = get_next_line_from_buf(buf, &i, n);
           if (line == NULL)
               break;

           int username_length = strchr(line, ' ') - line;
           char *substr = (char *)malloc(sizeof(char) * username_length);
           strncpy(substr, line, username_length);
           if (strcmp(substr, username) == 0)
               printf("FOUND");
           printf("%s\n", line);
           free(substr);
       }
       if (n <= 0)
           over = true;
       close(fd);
    }

    free(buf);
    free(line);
}

