#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, const char *argv[])
{
 //   char random_garbage[8192];
    int fd = -1;
   
    fd = open("sparce", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
        perror("Can't open file: ");
        return 2;
    }
    //write(fd, random_garbage, 8192);
    if (lseek(fd, 5 * 4096, SEEK_END) == -1)
    {
		perror("lseek");
	}   // write(fd, random_garbage, 8192);
    close(fd);
    return 0;
}                                                
