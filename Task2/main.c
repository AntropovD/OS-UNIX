#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <unistd.h>
#define _GNU_SOURCE

void execute_once(char ** argv)
void execute_repeat(char ** argv);
int create_daemon();//int argc, char *argv);

int main()
{
    FILE * fp;
    fp = fopen("config.txt", "r");
    if (fp == NULL)
    {
        printf("Config file not found");
        return 0;
    }
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    chdir("/");
    openlog("Dmitry's Task Manager", LOG_PID | LOG_CONS, LOG_DAEMON);
    syslog(LOG_INFO, "Task Manager started");

    while ((read = getline(&line, &len, fp)) != -1)
    {
        if (line[0] == '#')
            continue;
        char ** res  = NULL;
        char * p    = strtok (line, " ");
        int n_spaces = 0, i;

        while (p)
        {
          res = realloc (res, sizeof (char*) * ++n_spaces);
          if (res == NULL)
          {
              syslog(LOG_ERR, "Cannot allocate memory");
              exit (-1);
          }
          res[n_spaces-1] = p;
          p = strtok (NULL, " ");
        }
        res = realloc (res, sizeof (char*) * (n_spaces+1));
        res[n_spaces] = 0;

        char ** argv = malloc(sizeof (char*) * (n_spaces-2));
        for (i=0; i<n_spaces-2; i++)
            argv[i] = res[i];

        if (strcmp(res[n_spaces-1], "Once\n") == 0)
        {
            execute_once(argv);
        }
        else if (strcmp(res[n_spaces-1], "Repeat\n") == 0)
        {
            execute_repeat(argv);
        }
        else
        {
            syslog(LOG_ALERT, "Wrong type in config file.");
        }

        free(argv*);
        free (res);
    }

    fclose(fp);
    if (line)
      free(line);
    return 0;
}

void execute_once(char **argv)
{
    syslog(LOG_INFO, "New Process Once: %s", *argv);
    pid_t pid;
    switch(pid=fork())
    {
    case -1:
        syslog(LOG_ERR, "Cannot fork.");
        return;
    case 0:
    //CHILD
        return;

    default:
    //PARENT
        return;
    }
}

void execute_repeat(char **argv)
{
    //while (1)
    //{
        syslog(LOG_INFO, "New Process Repeat: %s", *argv);
   // }
}

int create_daemon()//int argc, char *argv)
{
    unsigned int fd;
    struct rlimit flim;

    if (getppid() != 1)
    {
        signal(SIGTTOU, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
        signal(SIGTTIN, SIG_IGN);
        if (fork() != 0)
            exit(0);
        setsid();
    }

    getrlimit(RLIMIT_NOFILE, &flim);
    for (fd=0; fd < flim.rlim_max; fd++)
        close(fd);
    chdir("/");
    openlog("Скелет демона", LOG_PID | LOG_CONS, LOG_DAEMON);
    syslog(LOG_INFO, "Демон начал работу...");
    closelog();

    return 0;
}
//        char *command = malloc( sizeof(*command) * 100);
//        strcpy(command, res[0]);
//        for (i = 1; i < (n_spaces-1); ++i)
//        {
//            strcat(command, " ");
//            strcat(command, res[i]);
//        }
