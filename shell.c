#include "builtin.h"
#include "ls.h"
#include "history.h"
#include "pinfo.h"
#include "discovery.h"
#include "prompt.h"
#include "background.h"
#include "foreground.h"
#include "helper.h"
#include "jobs.h"
#include "fgbg.h"
#include "autocomplete.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>

typedef long long int ll;

ll number[MAX_BACKGROUND_JOBS];
char backcommand[MAX_BACKGROUND_JOBS][MAX_COMMAND_LENGTH];
int backgroundpid[MAX_BACKGROUND_JOBS];
int fgthere = 0;
int fgpid;
char fname[MAX_COMMAND_LENGTH];
char fnumber;

extern ll sortednumber[MAX_BACKGROUND_JOBS];
extern char sortedbackcommand[MAX_BACKGROUND_JOBS][MAX_COMMAND_LENGTH];
extern int sortedbackgroundpid[MAX_BACKGROUND_JOBS];

void ctrlc()
{
    if (fgthere)
        printf("\n");
    return;
}

void ctrlz()
{
    if (fgthere)
    {
        if (kill(fgpid, SIGTSTP) < 0)
        {
            printf("CTRLZ failed\n");
        }
        for (ll pidno = 0; pidno < MAX_BACKGROUND_JOBS; pidno++)
        {
            if (backgroundpid[pidno] == -1)
            {
                strcpy(backcommand[pidno], fname);
                backgroundpid[pidno] = fgpid;
                number[pidno] = pidno + 1;
                break;
            }
        }
        return;
    }
}

void sort()
{
    char parameter[MAX_BACKGROUND_JOBS][MAX_COMMAND_LENGTH];
    for (ll i = 0; i < MAX_BACKGROUND_JOBS; i++)
    {
        int flag2 = 0;
        int charread = 0;

        sortednumber[i] = number[i];
        char temp[MAX_COMMAND_LENGTH];
        if (backgroundpid[i] != -1)
        {

            strcpy(temp, backcommand[i]);
            for (ll j = 0; j < MAX_COMMAND_LENGTH; j++)
            {
                if (temp[j] != ' ')
                    charread = 1;
                if (temp[j] == ' ' && charread)
                {
                    int c = 0;
                    for (ll k = 0; k < j; k++)
                    {
                        if (backcommand[i][k] == ' ')
                            continue;
                        parameter[i][c++] = backcommand[i][k];
                        if (k == j - 1)
                            parameter[i][c] = '\0';
                    }
                    flag2 = 1;
                    break;
                }
                if (flag2)
                    break;
            }
            strcpy(sortedbackcommand[i], backcommand[i]);
        }
        else
        {
            strcpy(sortedbackcommand[i], "\0");
        }
        sortedbackgroundpid[i] = backgroundpid[i];
    }

    for (ll i = 0; i < MAX_BACKGROUND_JOBS; i++)
    {
        ll j, k;
            int flag = 0;

        for (j = 0; j < MAX_BACKGROUND_JOBS - i; j++)
        {
            if (flag)
            break;
            if (sortedbackgroundpid[j] != -1)
            {
                for (k = j + 1; k < MAX_BACKGROUND_JOBS - i; k++)
                {
                    if (sortedbackgroundpid[k] != -1)
                        break;
                }
                if (k >= MAX_BACKGROUND_JOBS)
                {
                    flag = 1;
                    break;
                }
                if (strcmp(parameter[j], parameter[k]) > 0)
                {
                    char tempcommand[MAX_COMMAND_LENGTH] = {'\0'}, tempparameter[MAX_COMMAND_LENGTH] = {'\0'};
                    int tempbackgroundpid;
                    ll tempnumber;
                    strcpy(tempcommand, sortedbackcommand[j]);
                    strcpy(sortedbackcommand[j], sortedbackcommand[k]);
                    strcpy(sortedbackcommand[k], tempcommand);
                    strcpy(tempparameter, parameter[j]);
                    strcpy(parameter[j], parameter[k]);
                    strcpy(parameter[k], tempparameter);
                    tempnumber = sortednumber[j];
                    sortednumber[j] = sortednumber[k];
                    sortednumber[k] = tempnumber;
                    tempbackgroundpid = sortedbackgroundpid[j];
                    sortedbackgroundpid[j] = sortedbackgroundpid[k];
                    sortedbackgroundpid[k] = tempbackgroundpid;
                }
            }
        }
    }
}

void dojobs()
{
    char *parameter;
    ll stopped = 0, running = 0;
    parameter = strtok(NULL, " ");
    while (parameter != NULL)
    {
        if (parameter[0] == '-')
        {
            for (ll k = 0; k < strlen(parameter); k++)
            {
                if (parameter[k] == 's')
                    stopped = 1;
                else if (parameter[k] == 'r')
                    running = 1;
            }
        }
        parameter = strtok(NULL, " ");
    }
    if (!stopped && !running)
    {
        stopped = 1;
        running = 1;
    }
    sort();

    for (ll i = 0; i < MAX_BACKGROUND_JOBS; i++)
    {
        if (sortedbackgroundpid[i] != -1)
        {
            char pid[10];
            sprintf(pid, "%d", sortedbackgroundpid[i]);
            char procstat[MAX_COMMAND_LENGTH] = "/proc/";

            strcat(procstat, pid);

            strcat(procstat, "/stat");
            int fd = open(procstat, O_RDONLY);
            if (fd > 0)
            {
                char buf[MAX_BUFFER_SIZE];
                read(fd, buf, MAX_BUFFER_SIZE);
                char *x = strtok(buf, " ");
                close(fd);
                char a[2];

                for (int i = 2; i <= 23; i++)
                {
                    x = strtok(NULL, " ");

                    if (i == 3)
                        strcpy(a, x);
                }
                a[1] = '\0';
                char status[25];
                if (!strcmp(a, "T"))
                {
                    strcpy(status, "Stopped");
                    if (stopped)
                        printf("[%lld] %s %s [%d]\n", sortednumber[i], status, sortedbackcommand[i], sortedbackgroundpid[i]);
                }
                else
                {
                    strcpy(status, "Running");
                    if (running)
                        printf("[%lld] %s %s [%d]\n", sortednumber[i], status, sortedbackcommand[i], sortedbackgroundpid[i]);
                }
            }
        }
    }
}

int main()
{
    for (ll i = 0; i < MAX_BACKGROUND_JOBS; i++)
    {
        backgroundpid[i] = -1;
    }
    char home_directory[MAX_COMMAND_LENGTH];
    char old_directory[MAX_COMMAND_LENGTH];
    old_directory[0] = '\0';
    long int time = -1;
    getcwd(home_directory, sizeof(home_directory));
    char historypath[MAX_PATH_LENGTH];
    sprintf(historypath, "%s/%s", home_directory, "history.txt");
    int fd = open(historypath, O_RDWR);
    char history[HISTORY][MAX_COMMAND_LENGTH];
    int historyindex = 0, numofcommands = 0;
    historyindex = gethistoryfirst(history, &numofcommands, fd);
    close(fd);

    while (1)
    {
        signal(SIGCHLD, backgrounddone);
        signal(SIGINT, ctrlc);
        signal(SIGTSTP, ctrlz);
        char current_directory[MAX_COMMAND_LENGTH];
        prompt(home_directory, current_directory, time);
        time = -1;
        ll no_of_backgrounds = 0;
        char a[MAX_COMMAND_LENGTH], b[MAX_COMMAND_LENGTH];
        char readchar;
        setbuf(stdout, NULL);
        enableRawMode();
        memset(a, '\0', MAX_COMMAND_LENGTH);
        int pt = 0;
        while (read(STDIN_FILENO, &readchar, 1) == 1)
        {
            if (pt >= MAX_COMMAND_LENGTH - 1)
            {
                printf("\nCommand too long (max %d characters)\n", MAX_COMMAND_LENGTH - 1);
                pt = 0;
                memset(a, '\0', MAX_COMMAND_LENGTH);
                continue;
            }
            if (iscntrl(readchar))
            {
                if (readchar == 10)
                {
                    a[pt] = '\0';
                    break;
                }
                else if (readchar == 27)
                {
                    char buf[3];
                    buf[2] = 0;
                    if (read(STDIN_FILENO, buf, 2) == 2)
                    {
                        printf("\rarrow key: %s", buf);
                    }
                }
                else if (readchar == 127)
                { // backspace
                    if (pt > 0)
                    {
                        if (a[pt - 1] == 9)
                        {
                            for (int i = 0; i < 7; i++)
                            {
                                printf("\b");
                            }
                        }
                        a[--pt] = '\0';
                        printf("\b \b");
                    }
                }
                else if (readchar == 9)
                { // TAB character
                    pt = autocomplete(a, home_directory, pt);
                    pt = strlen(a);
                }
                else if (readchar == 4)
                {
                    printf("\n");
                    exit(0);
                }
                else
                {
                    printf("%d\n", readchar);
                }
            }
            else
            {
                a[pt++] = readchar;
                printf("%c", readchar);
            }
        }
        printf("\n");
        disableRawMode();

        strcpy(b, a);
        ll len = strlen(a);
        if (a[0] != '\0')
        {
            gethistory(history, historypath, a, &historyindex, &numofcommands);
        }

        char **commands = (char **)malloc(sizeof(char *) * 10);

        ll i = tokenize(a, commands, ";");

        ll j = 0;
        if (issyntaxerror(b, commands, i))
        {
            while (j <= i)
            {

                char *token[10];
                int flag = 0;
                if (commands[j][strlen(commands[j]) - 1] == '&')
                    flag = 1;

                ll l = tokenize(commands[j], token, "&");
                if (flag == 1)
                    l++;
                ll looping = 0;
                // printf("%s\n", command);

                while (looping < l)
                {
                    no_of_backgrounds++;

                    char **pipedcommands = (char **)malloc(sizeof(char *) * MAX_PIPED_COMMANDS);

                    ll noofpiped = tokenize(token[looping], pipedcommands, "|");

                    ll pipeloop = 0;
                    int pipeprev[2];
                    int pipenow[2];
                    while (pipeloop <= noofpiped)
                    {
                        int standardread, standardwrite;
                        if (pipeloop)
                        {
                            if (pipeloop > 1)
                            {
                                close(pipeprev[0]);
                                close(pipeprev[1]);
                            }
                            pipeprev[0] = pipenow[0];
                            pipeprev[1] = pipenow[1];
                            standardread = dup(STDIN_FILENO);
                            if (dup2(pipeprev[0], STDIN_FILENO) < 0)
                            {

                                perror("dup2 failed");
                                exit(1);
                            }
                        }
                        if (pipe(pipenow) < 0)
                        {
                            perror("pipe failed");
                            exit(1);
                        }
                        if (pipeloop != noofpiped)
                        {
                            standardwrite = dup(STDOUT_FILENO);
                            if (dup2(pipenow[1], STDOUT_FILENO) < 0)
                            {
                                perror("dup2 failed");
                                exit(1);
                            }
                        }
                        char *command;
                        int original, originalread;
                        int redirectfd, redirectedfdread;
                        int mask;
                        char b[MAX_COMMAND_LENGTH] = {'\0'}, c[MAX_COMMAND_LENGTH] = {'\0'}, d[MAX_COMMAND_LENGTH] = {'\0'}, e[MAX_COMMAND_LENGTH] = {'\0'};
                        strcpy(b, pipedcommands[pipeloop]);
                        strcpy(c, pipedcommands[pipeloop]);
                        strcpy(d, pipedcommands[pipeloop]);
                        strcpy(e, pipedcommands[pipeloop]);
                        for (ll count = 0; count < strlen(c); count++)
                        {
                            if (c[count] == '>')
                            {
                                if (c[count + 1] == '>')
                                    mask = O_APPEND;
                                else
                                    mask = O_TRUNC;
                                break;
                            }
                        }
                        int isredirected = 0, isredirectedread = 0;
                        char *redirect = strtok(c, ">");
                        redirect = strtok(NULL, ">");
                        if (redirect != NULL)
                        {
                            isredirected = 1;
                            redirect = strtok(redirect, "< ");
                            redirectfd = open(redirect, O_CREAT | O_WRONLY | mask, 0644);
                            if (redirectfd < 0)
                            {
                                perror("Failed to open file for redirection");
                                exit(1);
                            }
                            if ((original = dup(STDOUT_FILENO)) < 0)
                            {
                                perror("dup2 failed");
                                exit(1);
                            }
                            if (dup2(redirectfd, STDOUT_FILENO) < 0)
                            {
                                perror("dup2 failed");
                                exit(1);
                            }
                        }

                        char *redirectread = strtok(d, "<");
                        redirectread = strtok(NULL, "<");
                        if (redirectread != NULL)
                        {
                            isredirectedread = 1;
                            redirectread = strtok(redirectread, "> ");
                            redirectedfdread = open(redirectread, O_RDONLY);
                            if (redirectedfdread < 0)
                            {
                                perror("Failed to open input file");
                                exit(1);
                            }
                            if (redirectedfdread < 0)
                            {
                                perror("Failed to open file for redirection");
                                exit(1);
                            }
                            if ((originalread = dup(STDIN_FILENO)) < 0)
                            {
                                perror("dup2 failed");
                                exit(1);
                            }
                            if (dup2(redirectedfdread, STDIN_FILENO) < 0)
                            {
                                perror("dup2 failed");
                                exit(1);
                            }
                        }
                        command = strtok(b, ">>");
                        command = strtok(b, ">");
                        command = strtok(b, "<");
                        command = strtok(pipedcommands[pipeloop], ">>");
                        command = strtok(pipedcommands[pipeloop], ">");
                        command = strtok(pipedcommands[pipeloop], "<");
                        command = strtok(pipedcommands[pipeloop], "\t\n ");
                        // printf("%s\n", command);

                        if (command != NULL)
                        {

                            if (!(whichcommand(command, home_directory, old_directory)))
                            {
                for (ll pidno = 0; pidno < MAX_BACKGROUND_JOBS; pidno++)
                {
                    if (backgroundpid[pidno] == -1)
                    {
                        strcpy(backcommand[pidno], e);
                        background(b, pidno + 1);
                        break;
                    }
                }
                            }

                            if (isredirected)
                            {
                                close(redirectfd);
                                if (dup2(original, STDOUT_FILENO) < 0)
                                {
                                    perror("dup2 failed");
                                    exit(1);
                                }
                                // close(original);
                            }

                            if (isredirectedread)
                            {
                                close(redirectedfdread);
                                if (dup2(originalread, STDIN_FILENO) < 0)
                                {
                                    perror("dup2 failed");
                                    exit(1);
                                }
                                // close(originalread);
                            }
                        }
                        if (pipeloop)
                        {
                            if (dup2(standardread, STDIN_FILENO) < 0)
                            {
                                perror("dup2 failed");
                                exit(1);
                            }
                            close(standardread);
                            close(pipeprev[0]);
                        }
                        if (pipeloop != noofpiped)
                        {
                            if (dup2(standardwrite, STDOUT_FILENO) < 0)
                            {
                                perror("dup2 failed");
                                exit(1);
                            }
                            close(standardwrite);
                            close(pipenow[1]);
                        }

                        pipeloop++;
                    }
                    if (noofpiped)
                    {
                        close(pipenow[0]);
                        close(pipenow[1]);
                        close(pipeprev[0]);
                        close(pipeprev[1]);
                    }
                    looping++;
                }
                if (!flag)
                {
                    char **pipedcommands = (char **)malloc(sizeof(char *) * MAX_PIPED_COMMANDS);

                    ll noofpiped = tokenize(token[l], pipedcommands, "|");

                    ll pipeloop = 0;
                    int pipeprev[2];
                    int pipenow[2];
                    while (pipeloop <= noofpiped)
                    {
                        int standardread, standardwrite;
                        if (pipeloop)
                        {
                            if (pipeloop > 1)
                            {
                                close(pipeprev[0]);
                                close(pipeprev[1]);
                            }
                            pipeprev[0] = pipenow[0];
                            pipeprev[1] = pipenow[1];
                            standardread = dup(STDIN_FILENO);
                            if (dup2(pipeprev[0], STDIN_FILENO) < 0)
                            {

                                perror("dup2 failed");
                                exit(1);
                            }
                        }
                        if (pipe(pipenow) < 0)
                        {
                            perror("pipe failed");
                            exit(1);
                        }
                        if (pipeloop != noofpiped)
                        {
                            standardwrite = dup(STDOUT_FILENO);
                            if (dup2(pipenow[1], STDOUT_FILENO) < 0)
                            {
                                perror("dup2 failed");
                                exit(1);
                            }
                        }
                        char *command;
                        char b[MAX_COMMAND_LENGTH] = {'\0'}, c[MAX_COMMAND_LENGTH] = {'\0'}, d[MAX_COMMAND_LENGTH] = {'\0'};
                        int original, originalread;
                        int redirectfd, redirectedfdread;
                        int mask;
                        strcpy(b, pipedcommands[pipeloop]);
                        strcpy(c, pipedcommands[pipeloop]);
                        strcpy(d, pipedcommands[pipeloop]);
                        for (ll count = 0; count < strlen(c); count++)
                        {
                            if (c[count] == '>')
                            {
                                if (c[count + 1] == '>')
                                    mask = O_APPEND;
                                else
                                    mask = O_TRUNC;
                                break;
                            }
                        }
                        int isredirected = 0, isredirectedread = 0;
                        char *redirect = strtok(c, ">");
                        redirect = strtok(NULL, ">");
                        if (redirect != NULL)
                        {
                            isredirected = 1;
                            redirect = strtok(redirect, "< ");
                            redirectfd = open(redirect, O_CREAT | O_WRONLY | mask, 0644);
                            if (redirectfd < 0)
                            {
                                perror("Failed to open file for redirection");
                                exit(1);
                            }
                            if ((original = dup(STDOUT_FILENO)) < 0)
                            {
                                perror("dup2 failed");
                                exit(1);
                            }
                            if (dup2(redirectfd, STDOUT_FILENO) < 0)
                            {
                                perror("dup2 failed");
                                exit(1);
                            }
                        }
                        char *redirectread = strtok(d, "<");
                        redirectread = strtok(NULL, "<");
                        if (redirectread != NULL)
                        {
                            isredirectedread = 1;
                            redirectread = strtok(redirectread, "> ");
                            errno = 0;
                            redirectedfdread = open(redirectread, O_RDONLY);
                            if (redirectedfdread < 0)
                            {
                                perror("Failed to open input file");
                            }

                            if ((originalread = dup(STDIN_FILENO)) < 0)
                            {
                                perror("dup2 failed");
                                exit(1);
                            }
                            if (dup2(redirectedfdread, STDIN_FILENO) < 0)
                            {
                                perror("dup2 failed");
                                exit(1);
                            }
                        }
                        command = strtok(b, ">>");
                        command = strtok(b, ">");
                        command = strtok(b, "<");
                        command = strtok(pipedcommands[pipeloop], ">>");
                        command = strtok(pipedcommands[pipeloop], ">");
                        command = strtok(pipedcommands[pipeloop], "<");
                        command = strtok(pipedcommands[pipeloop], "\t\n ");
                        // printf("%s\n", command);
                        if (command != NULL)
                        {
                            if (!(whichcommand(command, home_directory, old_directory)))
                            {
                                fgthere = 1;
                                strcpy(fname, b);
                                time = foreground(b);
                                if (time < 1)
                                    time = -1;
                                fgthere = 0;
                                fgpid = -1;
                                memset(fname, '\0', MAX_COMMAND_LENGTH);
                            }
                            if (isredirected)
                            {
                                close(redirectfd);

                                if (dup2(original, STDOUT_FILENO) < 0)
                                {
                                    perror("dup2 failed");
                                    exit(1);
                                }
                                // close(original);
                            }
                            if (isredirectedread)
                            {
                                if (dup2(originalread, STDIN_FILENO) < 0)
                                {
                                    perror("dup2 failed");
                                    exit(1);
                                }
                                close(redirectedfdread);
                                // close(originalread);
                            }
                        }
                        if (pipeloop)
                        {
                            if (dup2(standardread, STDIN_FILENO) < 0)
                            {
                                perror("dup2 failed");
                                exit(1);
                            }
                            close(standardread);
                            close(pipeprev[0]);
                        }
                        if (pipeloop != noofpiped)
                        {
                            if (dup2(standardwrite, STDOUT_FILENO) < 0)
                            {
                                perror("dup2 failed");
                                exit(1);
                            }
                            close(standardwrite);
                            close(pipenow[1]);
                        }

                        pipeloop++;
                    }
                    if (noofpiped)
                    {
                        close(pipenow[0]);
                        close(pipenow[1]);
                        close(pipeprev[0]);
                        close(pipeprev[1]);
                    }
                }
                j++;
            }
        }
        else
        {
            printf("bash: syntax error near unexpected token\n");
        }
    }
}
