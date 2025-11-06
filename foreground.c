#include "foreground.h"
#include "helper.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <wait.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

extern int fg;
extern int fgpid;
extern char fname[MAX_COMMAND_LENGTH];
extern int fnumber;

long int foreground(char *command)
{
    int status;
    char *args[MAX_ARGS];
    char *parameter = strtok(command, "\t\n>< ");
    ll i = 0;
    while (parameter != NULL)
    {
        args[i] = (char *)malloc(strlen(parameter) + 1);
        if (args[i] == NULL)
        {
            perror("malloc failed");
            // Free already allocated args
            for (ll j = 0; j < i; j++)
            {
                free(args[j]);
            }
            return -1;
        }
        strcpy(args[i++], parameter);
        parameter = strtok(NULL, "\t\n>< ");
    }
    args[i] = NULL;
    time_t begin = time(NULL);
    int forkReturn = fork();
    if (forkReturn < 0)
    {
        perror("fork failed");
        // Free allocated memory
        for (ll j = 0; j < i; j++)
        {
            free(args[j]);
        }
        return -1;
    }
    if (forkReturn == 0)
    {
        errno = 0;
        execvp(args[0], args);
        if (errno == 2)
        {
            printf("Invalid command\n");
        }
        else
        {
            perror("");
        }
        // Free allocated memory before exit
        for (ll j = 0; j < i; j++)
        {
            free(args[j]);
        }
        errno = 0;
        exit(1);
    }
    else
    {
        fgpid = forkReturn;

        waitpid(forkReturn, &status, 0);
        time_t end = time(NULL);
        // Free allocated memory
        for (ll j = 0; j < i; j++)
        {
            free(args[j]);
        }
        return (end - begin);
    }
}