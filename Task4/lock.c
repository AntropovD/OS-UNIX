#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "lock.h"


void lock(char *filename, enum lock_type type)
{    
    char *lck_1st_level = concat_lck_extension(filename);
    char *lck_2nd_level = concat_lck_extension(lck_1st_level);

    // zero level
    if (!is_file_exist(lck_1st_level) && !is_file_exist(lck_2nd_level)) {
        get_descriptor_if_not_locked(filename, type);
    }
    // 1st level exists but second not
    else if (is_file_exist(lck_1st_level) && !is_file_exist(lck_2nd_level)) {
        get_descriptor_1st_level(filename, type);
    }
    // 1st level exists and 2nd too
    else if (is_file_exist(lck_2nd_level)) {
        get_descriptor_2nd_level(filename, type);
    }
    free(lck_1st_level);
    free(lck_2nd_level);
    return;
}

void get_descriptor_if_not_locked(char *filename, enum lock_type type)
{
    char *lck_1st_level = concat_lck_extension(filename);
    char *lck_2nd_level = concat_lck_extension(lck_1st_level);

    create_lck_file(lck_2nd_level, WRITE);
    create_lck_file(lck_1st_level, type);
    if (DEBUG) printf("DEBUG: Lock on file %s installed %d level 0\n",
               filename, (int)type);
    unlink(lck_2nd_level);

    free(lck_1st_level);
    free(lck_2nd_level);
    return;
}

void get_descriptor_1st_level(char *filename, enum lock_type type)
{
    char *lck_1st_level = concat_lck_extension(filename);
    char *lck_2nd_level = concat_lck_extension(lck_1st_level);

    create_lck_file(lck_2nd_level, WRITE);

    if (type == WRITE) {
        while (is_file_exist(lck_1st_level))
            sleep(1);
        unlink(lck_1st_level);
    }
    else if (type == READ) {
        if (!check_if_all_locks_type_are_read(lck_1st_level))
            while (is_file_exist(lck_1st_level))
                sleep(1);
    }
    create_lck_file(lck_1st_level, type);
    if (DEBUG) printf("DEBUG: Lock on file %s installed %d level 1\n",
           filename, (int)type);
    unlink(lck_2nd_level);

    free(lck_1st_level);
    free(lck_2nd_level);
}

void get_descriptor_2nd_level(char *filename, enum lock_type type)
{
    char *lck_1st_level = concat_lck_extension(filename);
    char *lck_2nd_level = concat_lck_extension(lck_1st_level);

    while (is_file_exist(lck_2nd_level)) {
        sleep(1);
    }
    create_lck_file(lck_2nd_level, WRITE);
    if (type == WRITE) {
        while (is_file_exist(lck_1st_level))
            sleep(1);
        unlink(lck_1st_level);
    }
    else if (type == READ) {
        if (!check_if_all_locks_type_are_read(lck_1st_level))
            while (is_file_exist(lck_1st_level))
                sleep(1);
    }

    create_lck_file(lck_1st_level, type);
    if (DEBUG) printf("DEBUG: Lock on file %s installed %d level 2\n",
           filename, (int)type);
    unlink(lck_2nd_level);

    free(lck_1st_level);
    free(lck_2nd_level);
}

void unlock(char * filename, enum lock_type type)
{
    char *lck_1st_level = concat_lck_extension(filename);

    if (type == WRITE)
        unlink(lck_1st_level);
    else if (type == READ) {
        delete_only_your_read_block_in_file(lck_1st_level);
    }
    free(lck_1st_level);
    printf("DEBUG: Lock on %s was uninstalled\n", filename);
}


void delete_only_your_read_block_in_file(char *lck_1st_level)
{
    char *tmp_filename = (char *)calloc(40, sizeof(char));
    strcpy(tmp_filename, lck_1st_level);
    strcat(tmp_filename, ".tmp");

    FILE *rd = fopen(lck_1st_level, "r");
    FILE *wd = fopen(tmp_filename, "w");

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int count = 0;
    char *pid_str = (char *)calloc(40, sizeof(char));
    sprintf(pid_str, "%ld", (long)getpid());

    while ((read = getline(&line, &len, rd)) != -1) {
        char *tmp_line = (char *)calloc(read, sizeof(char));
        strncpy(tmp_line, line, read);
        char *pid = strtok(line, " ");
        if (strcmp(pid, pid_str) != 0){
            fputs(tmp_line, wd);
            count++;
        }
        free(tmp_line);
    }

    fclose(rd);
    fclose(wd);
    unlink(lck_1st_level);
    if (count != 0)
        rename(tmp_filename, lck_1st_level);
    else
        unlink(tmp_filename);
    free(tmp_filename);

    free(line);
    free(pid_str);
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

bool check_if_all_locks_type_are_read(char *filename)
{
    FILE *stream = fopen(filename, "r");

    char *line = NULL;
    size_t len = 0;
    ssize_t read = 0;

    while ((read = getline(&line, &len, stream)) != -1) {
        strtok(line, " ");
        char *type = strtok(NULL, " ");
        if (strcmp(type, "Read\n") != 0) {
            fclose(stream);
            return false;
        }
    }
    if (line) free(line);
    fclose(stream);
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

