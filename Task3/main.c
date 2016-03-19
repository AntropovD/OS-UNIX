#include <stdio.h>
#include <fcntl.h>

int main(int argc, char ** argv) {	

	if (argc != 2) {
		printf("Gzip addition for sparced files\n");
		printf("Usage: gzip -d file.gz -c | %s OutputFile", argv[0]);
		return 1;
	}
	int file;
	if ((file = open(argv[1], O_WRONLY | O_BINARY) == -1) {
		printf("Cannot open file.\n");
		return 1;
	}

	char c;
    int count = 0;
	while ((c = getchar()) != EOF) {
		if (c == 0)
            lseek(file, 1, SEEK_CUR);
		else 
			write(file, c, 1);
	}
	close(file);
	return 0;
}
