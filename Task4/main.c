#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>




enum lockType{ Read, Write };
typedef enum lockType lockType;

char * concatLckStr(char * source);

int doesFileExist(char *filename);

void createLckFile(char *name, lockType type);


int main(int argc, char ** argv) {

    if (argc != 4) {
        printf("Change username and password in passwd file with own file blocking system\n");
        printf("Usage: %s filename username new_password\n", argv[0]);
        return 1;
    }
    char *file = argv[1];
    char *usr = argv[2];
    char *pwd = argv[3];

    if (!doesFileExist(file)) {
        printf("File not found.\n");
        return 1;
    }

    if (strlen(file) > 4 && strcmp(file + (strlen(file)-4), ".lck")==0) {
        printf("File is already locked.\n");
        return 1;
    }

    char *lckFile = concatLckStr(file);
    createLckFile(lckFile, Read);


    free(lckFile);
    return 0;
}

char * concatLckStr(char * source) {
    char *lckStr = malloc(sizeof(char)*256);
    strcpy(lckStr, source);
    strcat(lckStr, ".lck");
    return lckStr;
}

int doesFileExist(char *filename) {
    struct stat st;
    int result = stat(filename, &st);
    return result == 0;
}

void createLckFile(char *name, lockType type) {
    int file;
    file = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0664);
    char *buff = calloc(256, sizeof(char));
    sprintf(buff, "%ld", (long)getpid());
    printf("%s", buff);
    if (type == Read)
        strcat(buff, " read");
    else if (type == Write)
        strcat(buff, " write");
    write(file, buff, strlen(buff));
    close(file);
   free(buff);
}
