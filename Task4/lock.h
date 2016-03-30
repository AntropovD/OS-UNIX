#ifndef LOCK_H
#define LOCK_H
#include <stdbool.h>

#define DEBUG true
#define SLEEP true

enum lock_type {
    READ,
    WRITE
};

bool check_lck_file_for_type(char *name, enum lock_type type);
bool check_if_all_locks_type_are_read(char *filename);

char *concat_lck_extension(char * source);

int get_descriptor_if_not_locked(char *filename, enum lock_type type);
int get_descriptor_1st_level(char *filename, enum lock_type type);
int get_descriptor_2nd_level(char *filename, enum lock_type type);

int is_file_exist(char *filename);
int open_filename_with_type(char *filename, enum lock_type type);


void create_lck_file(char *name, enum lock_type type);
void delete_only_your_read_block_in_file(char *lck_1st_level);

void lock(char *filename, int *fd, enum lock_type type);
void unlock(char *filename, int fd, enum lock_type type);

#endif // LOCK_H
