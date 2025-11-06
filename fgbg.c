#include "fgbg.h"
#include "helper.h"
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern ll number[MAX_BACKGROUND_JOBS];
extern char backcommand[MAX_BACKGROUND_JOBS][MAX_COMMAND_LENGTH];
extern int backgroundpid[MAX_BACKGROUND_JOBS];

extern int fgthere;
extern int fgpid;
extern char fname[MAX_COMMAND_LENGTH];
extern int fnumber;

void fg()
{
    char *parameter = strtok(NULL, " \t\n");
    int jobnumber = atoi(parameter);
    int pid;
    ll i;

    for (i = 0; i < MAX_BACKGROUND_JOBS; i++)
    {
        if (number[i] == jobnumber)
        {
            pid = backgroundpid[i];
            break;
        }
    }
    if (i >= MAX_BACKGROUND_JOBS)
    {
        printf("Invalid pid\n");
        return;
    }
    strcpy(fname, backcommand[i]);
    fgthere = 1;
    fgpid = pid;
    fnumber = number[i];
    number[i] = 0;
    backgroundpid[i] = -1;
    for (ll j = 0; j < strlen(backcommand[i]); j++)
    {
        backcommand[i][j] = '\0';
    }

    setpgid(pid, getpgid(0));

    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    
    int status;

    tcsetpgrp(0, pid);

    kill(pid, SIGCONT);

    waitpid(pid, &status, WUNTRACED);

    tcsetpgrp(0, getpgid(0));

    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);

    if (WIFEXITED(status) || WIFSTOPPED(status))
    {
        fgthere = 0;
        return;
    }
}

void sig()
{
    char *parameter = strtok(NULL, " ");
    int jobnumber = atoi(parameter);
    parameter = strtok(NULL, " ");
    int y = atoi(parameter);

    int pid;
    ll i;
    for (i = 0; i < MAX_BACKGROUND_JOBS; i++)
    {
        if (number[i] == jobnumber)
        {
            pid = backgroundpid[i];
            break;
        }
    }
    if (i >= MAX_BACKGROUND_JOBS)
    {
        printf("Invalid pid\n");
        return;
    }
    if (kill(pid, y) < 0)
    {
        printf("Invalid sig\n");
        return;
    }
}

void bg()
{
    char *parameter = strtok(NULL, " \t\n");
    int jobnumber = atoi(parameter);
    int pid;
    ll i;

    for (i = 0; i < MAX_BACKGROUND_JOBS; i++)
    {
        if (number[i] == jobnumber)
        {
            pid = backgroundpid[i];
            break;
        }
    }

    if (i >= MAX_BACKGROUND_JOBS)
    {
        printf("Invalid pid\n");
        return;
    }

    if (kill(pid, SIGCONT) < 0)
        printf("bg error\n");
}