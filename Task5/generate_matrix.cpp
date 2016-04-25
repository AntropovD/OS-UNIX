#include <stdio.h>
#include <stdlib.h>
#include <time.h>


const int N = 15;
const int M = 8;
const int K = 13;
const char *outputFile = "input";

//Generate matrix NxM and MxK into file
int main()
{		
	srand(time(NULL));	
	FILE *fd = fopen(outputFile, "w+");	
	
	fprintf(fd, "%d %d\n", N, M);
	for (int i=0; i<N; i++) {
		for (int j=0; j<M; j++)
			fprintf(fd, "%d ", (rand()%200)-100);
		fprintf(fd, "\n");
	}
	fprintf(fd, "%d %d\n", M, K);
	for (int i=0; i<M; i++) {
		for (int j=0; j<K; j++)
			fprintf(fd, "%d ", (rand()%200)-100);
		fprintf(fd, "\n");
	}
			
	return 0;
}
