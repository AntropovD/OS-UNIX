#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h> 

#define BUF_SIZE 1024

int main(int argc, char ** argv) {	

	int output = open("out", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (output == -1) {
		perror("2");
		return 2;
	}
	
	int n, i;
	char buff[BUF_SIZE];	
	while((n = read(STDIN_FILENO, buff, BUF_SIZE)) > 0) {
		char *ptr = buff;
		for (i=0; i<n; i++) {
			if (buff[i] == 0) {
				lseek(output, 1, SEEK_END);
			}
			else {
				write(output, ptr, 1);							
				ptr++;
			}
		}			
	}
	close(output);
	return 0;
}
