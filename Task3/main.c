#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h> 

int main(int argc, char ** argv) {	

	if (argc != 2) {
		printf("Gzip addition for sparced files\n");
		printf("Usage: gzip -d file.gz -c | %s output\n", argv[0]);
		return 1;	
    }
    
	int file = -1;
	if ((file = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1) {
		printf("Cannot open file.\n");
		return 1;
	}

	char c[1];    
	int count = 0;;
	while ((c[0] = getchar()) != EOF) {
		if (c[0] == 0) {
            count++;
		}
		else {
			if (count !=0) {
				lseek(file, count, SEEK_CUR);
				count = 0;
			}				
            printf("%c", c[0]);
			write(file, c, 1);
        }
	}
	close(file);
	return 0;
}
