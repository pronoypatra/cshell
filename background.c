#include "background.h"
#include "helper.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

extern ll number[MAX_BACKGROUND_JOBS];
extern char backcommand[MAX_BACKGROUND_JOBS][MAX_COMMAND_LENGTH];
extern int backgroundpid[MAX_BACKGROUND_JOBS];

void backgrounddone()
{
    int status;

    int pid = waitpid(-1, &status, WNOHANG);
    ll i;
    if (pid > 0)
    {
        if (WIFSTOPPED(status))
            return;
        for (i = 0; i < MAX_BACKGROUND_JOBS; i++)
        {

            if (backgroundpid[i] == pid)
                break;
        }
        if(i >= MAX_BACKGROUND_JOBS)
            return;
        // assert(1 == 2);
        if (WIFEXITED(status))
        {
            char buf[MAX_BUFFER_SIZE];
            sprintf(buf, "[%lld] %s with pid = %d exited normally\n", number[i], backcommand[i], backgroundpid[i]);
            write(2, buf, strlen(buf));
            number[i] = 0;
            backgroundpid[i] = -1;
            for (ll j = 0; j < strlen(backcommand[i]); j++)
            {
                backcommand[i][j] = '\0';
            }
        }
        else
        {
            char buf[MAX_BUFFER_SIZE];
            sprintf(buf, "[%lld] %s with pid = %d exited abnormally\n", number[i], backcommand[i], backgroundpid[i]);
            write(2, buf, strlen(buf));
            number[i] = 0;
            backgroundpid[i] = -1;
            for (ll j = 0; j < strlen(backcommand[i]); j++)
            {
                backcommand[i][j] = '\0';
            }
        }
    }
    errno = 0;
}

void background(char *command, ll n)
{

    // printf    // printf("%s\n", execcommand);
    int forkReturn = fork();
    char printarguement[MAX_COMMAND_LENGTH];

    number[n - 1] = n ;
    char *args[MAX_ARGS];
    char *parameter = strtok(command, "\t\n>< ");
    ll num_of_args = 0;
    while (parameter != NULL)
    {
        args[num_of_args] = (char *)malloc(strlen(parameter) + 1);
        if (args[num_of_args] == NULL)
        {
            perror("malloc failed");
            // Free already allocated args
            for (ll j = 0; j < num_of_args; j++)
            {
                free(args[j]);
            }
            return;
        }
        strcpy(args[num_of_args++], parameter);
        // printf("%s\n", args[i]);
        parameter = strtok(NULL, "\t\n>< ");
    }
    args[num_of_args] = NULL;
    // printf("[%lld] %d\n", n, getpid());
    errno = 0;
    strcpy(printarguement, args[0]);

    backgroundpid[n - 1] = forkReturn;
    if(forkReturn < 0)
    {
        perror("fork failed");
        // Free allocated memory
        for (ll j = 0; j < num_of_args; j++)
        {
            free(args[j]);
        }
        return;
    }

    else if (forkReturn == 0)
    {
        setpgid(0,0);
        if (execvp(args[0], args) < 0)
        {
            printf("%s Invalid command\n", args[0]);
            // Free allocated memory before exit
            for (ll j = 0; j < num_of_args; j++)
            {
                free(args[j]);
            }
            exit(1);
        }
    }
    else
    {
        char pidstatus[MAX_COMMAND_LENGTH];
        sprintf(pidstatus,"[%lld] %d\n", n, forkReturn);
        write(2,pidstatus, strlen(pidstatus));
        // Free allocated memory in parent process
        for (ll j = 0; j < num_of_args; j++)
        {
            free(args[j]);
        }
    }
}