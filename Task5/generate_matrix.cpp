#include <stdio.h>
#include <stdlib.h>
#include <time.h>


const int N = 20;
const char *outputFile = "input";

//Generate 2 matrix NxN 
int main()
{		
	srand(time(NULL));	
	FILE *fd = fopen(outputFile, "w+");	
	
    fprintf(fd, "%d\n", N);
	for (int i=0; i<N; i++) {
		for (int j=0; j<N; j++)
			fprintf(fd, "%d ", (rand()%100)-50);
		fprintf(fd, "\n");
	}	
    fprintf(fd, "%d\n", N);
	for (int i=0; i<N; i++) {
		for (int j=0; j<N; j++)
			fprintf(fd, "%d ", (rand()%100)-50);
		fprintf(fd, "\n");
	}			
	return 0;
}
