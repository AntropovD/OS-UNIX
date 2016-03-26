#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "lock.h"

bool check_file_exists_and_not_locked(char *filename)
{
    if (!is_file_exist(filename)) {
        perror("File not found.\n");
        return false;
    }
    if (strlen(filename)>4 && strcmp(filename + (strlen(filename)-4), ".lck")==0) {
        perror("Cannot work on already locked files.\n");
        return false;
    }
    return true;
}

char *concat_lck_extension(char *source)
{
    char *lckStr = malloc(sizeof(char)*256);
    strcpy(lckStr, source);
    strcat(lckStr, ".lck");
    return lckStr;
}


int is_file_exist(char *filename)
{
    struct stat st;
    return stat(filename, &st) == 0;
}

void lock(int fd)
{

}

void create_lck_file(char *name, enum lock_type type)
{
    int file = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0664);
    char *buff = calloc(256, sizeof(char));

    sprintf(buff, "%ld", (long)getpid());
    if (type==READ)
        strcat(buff, " read");
    else if (type==WRITE)
        strcat(buff, " write");

    write(file, buff, strlen(buff));
    close(file);

    free(buff);
}


