#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <unistd.h>
#include<sys/wait.h>

typedef struct node {
    int val;
    struct node * next;
} node_t;

node_t *head = NULL;
node_t *curr = NULL;

void execute_once(char **argv);
void execute_repeat(char **argv);
void push(int val);
void print_list();
void remove_list(int val);
void start_task(char **tokens, int size);

int main(){
    FILE * fp;
    fp = fopen("config.txt", "r");
    if (fp == NULL){
        printf("Config file not found");
        fclose(fp);
        return 0;
    }
    chdir("/");
    openlog("Task Manager by Dantre", LOG_PID | LOG_CONS, LOG_DAEMON);
    syslog(LOG_INFO, "Task Manager started");

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, fp)) != -1){
        if (line[0] == '#')
            continue;
        char ** tokens  = NULL;
        char * p    = strtok (line, " ");
        int size = 0;

        while (p){
            tokens = realloc (tokens, sizeof (char*) * ++size);
            if (tokens == NULL){
                syslog(LOG_ERR, "Cannot allocate memory");
                exit (-1);
            }
            tokens[size-1] = p;
            p = strtok (NULL, " ");
        }
        tokens = realloc (tokens, sizeof (char*) * (size+1));
        tokens[size] = 0;

        start_task(tokens, size);

        free(tokens);
    }

    print_list(head);
    fclose(fp);
    if (line)
        free(line);
    return 0;
}

void start_task(char **tokens, int size){
    int i;
    char ** argv = malloc(sizeof (char*) * (size));
    for (i = 0; i < size-1; i++)
        argv[i] = tokens[i];

    if (strcmp(tokens[size-1], "Once\n") == 0)
        execute_once(argv);
    else if (strcmp(tokens[size-1], "Repeat\n") == 0)
        execute_repeat(argv);
    else
        syslog(LOG_ALERT, "Wrong type in config file.");
    free(argv);
}

void execute_once(char **argv){
    pid_t pid;

    if ((pid = fork()) < 0){
         syslog(LOG_ERR, "Forking child process failed");
         return;
    }
    else if (pid == 0){
        pid_t task_pid = fork();
        push(task_pid);
        int status;
        if (task_pid==0){
            syslog(LOG_INFO, "! %s started", *argv);
            execvp(*argv, argv);
            exit(0);
        }
        else{
            waitpid(task_pid, NULL, 0);
            syslog(LOG_INFO, "# %s completed", *argv);
            char *file = (char*)malloc(sizeof(char)*40);
            sprintf(file, "/tmp/%s.pid", *argv);
            remove(file);
            remove_list(pid);
            remove_list(task_pid);
            exit(0);
        }
    }
    else{
        push(pid);
        FILE *fs;
        char *file = (char*)malloc(sizeof(char)*40);
        sprintf(file, "/tmp/%s.pid", *argv);
        fs = fopen(file, "a");
        fprintf(fs, "%d\n", pid);
        fclose(fs);
    }
}

void execute_repeat(char **argv)
{

}

void remove_list(int val){
   if (head == NULL)
        return;
    if (head->val == val){
        head = head->next;
        return;
    }
    node_t *ptr = head->next;
    node_t *prev = head;

    while(ptr != NULL){
        if (ptr->val == val)
        {
            prev->next = ptr->next;
            break;
        }
        prev = ptr;
        ptr = ptr->next;
    }
}

void push(int val){
    if (head == NULL){
        node_t *ptr = (node_t*)malloc(sizeof(node_t));
        ptr->val = val;
        ptr->next = NULL;
        head = curr = ptr;
        return;
    }
    node_t *ptr = malloc(sizeof(node_t));
    ptr->val = val;
    ptr->next = NULL;

    curr-> next = ptr;
    curr = ptr;
}

void print_list() {
    node_t * current = head;

    while (current != NULL) {
        printf("%d\n", current->val);
        current = current->next;
    }
}

//int create_daemon()//int argc, char *argv)
//{
//    unsigned int fd;
//    struct rlimit flim;
//
//    if (getppid() != 1){
//        signal(SIGTTOU, SIG_IGN);
//        signal(SIGTSTP, SIG_IGN);
//        signal(SIGTTIN, SIG_IGN);
//        if (fork() != 0)
//            exit(0);
//        setsid();
//    }
//
//    getrlimit(RLIMIT_NOFILE, &flim);
//    for (fd=0; fd < flim.rlim_max; fd++)
//        close(fd);
//    chdir("/");
//    openlog("Скелет демона", LOG_PID | LOG_CONS, LOG_DAEMON);
//    syslog(LOG_INFO, "Демон начал работу...");
//    closelog();
//
//    return 0;
//}
