#ifndef LOCK_H
#define LOCK_H
#include <stdbool.h>

enum lock_type {
    READ,
    WRITE
};

bool check_lck_file_for_type(char *name, enum lock_type type);

char *concat_lck_extension(char * source);

int is_file_exist(char *filename);

void create_lck_file(char *name, enum lock_type type);

void lock(char *filename, int *fd, enum lock_type type);
void unlock(char *filename, int fd, enum lock_type type);

#endif // LOCK_H
