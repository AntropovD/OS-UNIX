#ifndef LOCK_H
#define LOCK_H
#include <stdbool.h>

enum lock_type {
    READ,
    WRITE
};

bool check_lck_file_for_type(char *name, enum lock_type type);

char *concat_lck_extension(char * source);

int get_descriptor_if_not_locked(char *filename, enum lock_type type);
int get_descriptor_1st_level(char *filename, enum lock_type type);
int get_descriptor_2nd_level(char *filename, enum lock_type type);

int is_file_exist(char *filename);
int open_filename_with_type(char *filename, enum lock_type type);


void create_lck_file(char *name, enum lock_type type);

void lock(char *filename, int *fd, enum lock_type type);
void unlock(char *filename, int fd, enum lock_type type);

#endif // LOCK_H
