#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/resource.h>


struct ConfigRecord{
	char ProgramName[40];
	char Parameters[100];
	char Type[40];
};

void ReadConfigFile();

int main(int argc, char **argv)
{
	int fd;
	struct rlimit flim;
	if (getppid() != 1)
	{
		signal (SIGTTOU, SIG_IGN);
		signal (SIGTTIN, SIG_IGN);
		signal (SIGTSTP, SIG_IGN);
		
	ReadConfigFile();
    return 0;
}

void ReadConfigFile()
{
	FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("config", "r");
    if (fp == NULL)
      return;

    while ((read = getline(&line, &len, fp)) != -1) {
		if (line[0] == '#')
			continue;
		char ** res  = NULL;
		char *  p    = strtok (line, " ");
		int n_spaces = 0;		
		printf("%d", n_spaces);
		while (p) {
		  res = realloc (res, sizeof (char*) * ++n_spaces);
		  if (res == NULL)
			exit (-1);
		  res[n_spaces-1] = p;
		  p = strtok (NULL, " ");
		}

		res = realloc (res, sizeof (char*) * (n_spaces+1));
		res[n_spaces] = 0;

		for (int i = 0; i < (n_spaces+1); ++i)
		  printf ("res[%d] = %s ", i, res[i]);
		printf("\n");
		free (res);
    }

    fclose(fp);
    if (line)
	  free(line);
}


