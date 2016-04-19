#include <algorithm>
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pthread.h>
#include <vector>
#include <unistd.h>

#define SORT_TIME 10

std::vector<long long> numbers;
pid_t pid;

bool is_digit(char c);
void extract_numbers(char *filename);
void kill_pid(int sig);
void sort_numbers();
void write_numbers(char *filename);
void *make_sort();

int main(int argc, char **argv)
{
    if (argc == 1) {
        printf("Sort numbers from many files\n");
        printf("Usage: %s file1 file2 ... resultFile\n", argv[0]);
        return 0;
    }
    int i;
    for (i = 1; i < argc - 1; i++) {
        extract_numbers(argv[i]);
    }

    sort_numbers();

    char *result_file = argv[argc - 1];
    write_numbers(result_file);

    return 0;
}


void extract_numbers(char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd == 0) {
    	printf("Error on file %s reading\n", filename);
    	exit(1);
    }

    int n;
    char digit;
    long long number = 0;
    int factor = 10;
    while ((n = read(fd, &digit, sizeof(char))) > 0) {
    	if (is_digit(digit)) {
    		number = (number * factor) + (digit - '0');
    	}
    	else {
    		if (number != 0) {
 				numbers.push_back(number);
 				number = 0;			
    		}
    	}
    }
}

void kill_pid(int sig)
{
	kill(pid, SIGTERM);
	printf("Sort functions waits too long, seems to be broken\n");
	exit(2);
}

void sort_numbers()
{
	signal(SIGALRM, kill_pid);

  	switch(pid = fork())
    {
        case 0:
        	sleep(3);
            std::sort(numbers.begin(), numbers.end());
            break;            
        case -1:
            printf("Cannot fork\n");
            exit(2);
        default:            
            alarm(SORT_TIME);            
            wait(NULL);
            exit(0);
    }

}

void write_numbers(char *filename)
{
	int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1) {
		printf("Error on file %s writing\n", filename);
    	exit(1);
	}
	FILE *stream = fdopen(fd, "w");

	for(std::vector<long long>::iterator it = numbers.begin(); it != numbers.end(); ++it) {
		fprintf(stream, "%lld ", *it);
	}
	fclose(stream);
	close(fd);
}

bool is_digit(char c)
{
	return c >= '0' && c <= '9';
}