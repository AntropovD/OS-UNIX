#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "lock.h"

void lock(char *filename, int *fd, enum lock_type type)
{    
    char *lck_1st_level = concat_lck_extension(filename);
    char *lck_2nd_level = concat_lck_extension(lck_1st_level);

    if (!is_file_exist(lck_1st_level) && !is_file_exist(lck_2nd_level)) {
        *fd = get_descriptor_if_not_locked(filename, type);
        free(lck_1st_level);
        free(lck_2nd_level);
        return;
    }

    // 1st level exists but second not
    if (is_file_exist(lck_1st_level) && !is_file_exist(lck_2nd_level)) {
        *fd = get_descriptor_1st_level(filename, type);
        free(lck_1st_level);
        free(lck_2nd_level);
        return;
    }
    // 1st level exists and 2nd too
    if (is_file_exist(lck_1st_level) && is_file_exist(lck_2nd_level)) {
        *fd = get_descriptor_2nd_level(filename, type);
        free(lck_1st_level);
        free(lck_2nd_level);
        return;
    }    

    if (!is_file_exist(lck_1st_level) && is_file_exist(lck_2nd_level)) {
        printf("ERROR: Created 2nd level block without 1st level!\n");
        free(lck_1st_level);
        free(lck_2nd_level);
        exit(2);
    }
}

int get_descriptor_if_not_locked(char *filename, enum lock_type type)
{
    char *lck_1st_level = concat_lck_extension(filename);
    char *lck_2nd_level = concat_lck_extension(lck_1st_level);

    create_lck_file(lck_2nd_level, WRITE);
    create_lck_file(lck_1st_level, type);
    printf("DEBUG: Lock on file %s installed %d level 0\n", filename, (int)type);
    unlink(lck_2nd_level);

    free(lck_1st_level);
    free(lck_2nd_level);
    return open_filename_with_type(filename, type);
}

int get_descriptor_1st_level(char *filename, enum lock_type type)
{
    char *lck_1st_level = concat_lck_extension(filename);
    char *lck_2nd_level = concat_lck_extension(lck_1st_level);

    create_lck_file(lck_2nd_level, WRITE);
    while (is_file_exist(lck_1st_level)) {
        sleep(1);
    }
    create_lck_file(lck_1st_level, type);
    printf("DEBUG: Lock on file %s installed %d level 1\n", filename, (int)type);
    unlink(lck_2nd_level);

    free(lck_1st_level);
    free(lck_2nd_level);
    return open_filename_with_type(filename, type);
}

int get_descriptor_2nd_level(char *filename, enum lock_type type)
{
    char *lck_1st_level = concat_lck_extension(filename);
    char *lck_2nd_level = concat_lck_extension(lck_1st_level);

    while (is_file_exist(lck_2nd_level)) {
        sleep(1);
    }
    create_lck_file(lck_2nd_level, WRITE);
    while (is_file_exist(lck_1st_level)) {
        sleep(1);
    }
    create_lck_file(lck_1st_level, type);
    printf("DEBUG: Lock on file %s installed %d way 3\n", filename, (int)type);
    unlink(lck_2nd_level);

    free(lck_1st_level);
    free(lck_2nd_level);
    return open_filename_with_type(filename, type);;
}

void unlock(char * filename, int fd, enum lock_type type)
{    
    close(fd);
    char *lck_1st_level = concat_lck_extension(filename);
    char *lck_2nd_level = concat_lck_extension(lck_1st_level);
    unlink(lck_1st_level);
    unlink(lck_2nd_level);
    free(lck_1st_level);
    free(lck_2nd_level);
    printf("DEBUG: Lock on %s was uninstalled\n", filename);
}

void create_lck_file(char *name, enum lock_type type)
{
    int file = open(name, O_WRONLY | O_CREAT | O_APPEND, 0664);
    char *buff = calloc(256, sizeof(char));

    if (type==READ)
        sprintf(buff, "%ld Read\n", (long)getpid());
    else if (type==WRITE)
        sprintf(buff, "%ld Write\n", (long)getpid());

    write(file, buff, strlen(buff));
    close(file);
    free(buff);
}

//  Проверяет если все локи типа read и нужен read
//  то дает разрешение
bool check_lck_file_for_type(char *filename, enum lock_type type)
{
    if (!is_file_exist(filename))
        return true;
    if (type == WRITE)
        return false;
    int file = open(filename, O_RDONLY);
    FILE *stream = fdopen(file, "r");

    char *line = NULL;
    size_t len = 0, read = 0;

    while ((read = getline(&line, &len, stream)) != -1) {
        char *pid = strtok(line, " ");
        char *type = strtok(NULL, " ");
        if (strcmp(type, "Read\n") != 0) {
            fclose(stream);
            close(file);
            return false;
        }
    }
    if (line) free(line);
    fclose(stream);
    close(file);
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

int open_filename_with_type(char *filename, enum lock_type type)
{
    if (type == WRITE)
        return open(filename, O_WRONLY);
    else
        return open(filename, O_RDONLY);
}
