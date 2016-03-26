#ifndef LOCK_H
#define LOCK_H
#include <stdbool.h>

enum lock_type {
    READ,
    WRITE
};

bool check_file_exists_and_not_locked(char *filename);

char *concat_lck_extension(char * source);

int is_file_exist(char *filename);

void create_lck_file(char *name, enum lock_type type);
void lock(int fd);

#endif // LOCK_H
