#include "builtin.h"
#include "ls.h"
#include "history.h"
#include "pinfo.h"
#include "discovery.h"
#include "prompt.h"
#include "background.h"
#include "foreground.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

// check tokenization of b with execvp
// check setpgid in background

typedef long long int ll;

int main()
{
    char home_directory[100];
    char old_directory[100];
    old_directory[0] = '\0';
    long int time = -1;
    getcwd(home_directory, sizeof(home_directory));
    char historypath[200];
    sprintf(historypath, "%s/%s", home_directory, "history.txt");
    int fd = open(historypath, O_RDWR);
    char history[HISTORY][100];
    int historyindex = 0, numofcommands = 0;
    int validread = 1;
    while (validread > 0)
    {
        char buf[2000];
        int l = 0;
        validread = read(fd, buf, sizeof(buf) - 1);
        if (validread > 0)
        {
            buf[validread] = '\0';
            for (int k = 0; k < validread; k++)
            {
                if (buf[k] == '\n')
                {
                    for (int x = l; x < k; x++)
                    {
                        history[historyindex][x - l] = buf[x];
                    }
                    history[historyindex][k - l] = '\0';
                    l = k + 1;
                    historyindex = (historyindex + 1) % HISTORY;
                    numofcommands++;
                }
            }
        }
    }
    while (1)
    {
        signal(SIGCHLD, backgrounddone);

        char current_directory[100];
        prompt(home_directory, current_directory, time);
        time = -1;
        ll no_semicolons = 0, no_ands = 0, no_commands = 0, no_of_backgrounds = 0;
        char a[100], b[100];
        fgets(a, 100, stdin);
        a[strlen(a) - 1] = '\0';
        strcpy(b, a);
        ll len = strlen(a);
        if (a[0] != '\0')
        {

            if (numofcommands < HISTORY)
            {
                if (strcmp(a, history[historyindex - 1]))
                {
                    int fd = open(historypath, O_RDWR | O_TRUNC);

                    strcpy(history[historyindex], a);
                    history[historyindex][strlen(history[historyindex])] = '\0';
                    historyindex = (historyindex + 1) % HISTORY;
                    numofcommands++;
                    for (int i = 0; i < numofcommands; i++)
                    {

                        char buf[102];
                        strcpy(buf, history[i]);
                        buf[strlen(history[i])] = '\n';
                        buf[strlen(history[i]) + 1] = '\0';
                        write(fd, buf, strlen(buf));
                    }
                }
                close(fd);
            }
            else
            {
                if (strcmp(history[((historyindex - 1) % HISTORY + HISTORY) % HISTORY], a))
                {
                    int fd = open(historypath, O_RDWR | O_TRUNC);

                    strcpy(history[historyindex], a);
                    history[historyindex][strlen(history[historyindex])] = '\0';

                    historyindex = (historyindex + 1) % HISTORY;
                    int j = historyindex;
                    for (int i = 0; i < HISTORY; i++)
                    {
                        char buf[102];
                        strcpy(buf, history[j]);
                        buf[strlen(history[j])] = '\n';
                        buf[strlen(history[j]) + 1] = '\0';
                        write(fd, buf, strlen(buf));
                        j = (j + 1) % HISTORY;
                    }
                }
                close(fd);
            }
        }
        for (ll k = 0; k < len; k++)
        {
            if (a[k] == ';')
                no_semicolons++;
            else if (a[k] == '&')
                no_ands++;
        }
        char **commands = (char **)malloc(sizeof(char *) * 10);

        commands[0] = strtok(a, ";");

        ll i = 0;
        // printf("%lld\n", i);
        // printf("%s\n", commands[0]);

        while (commands[i++] != NULL)
        {
            commands[i] = strtok(NULL, ";");
        }

        i -= 2;
        for (ll k = 0; k <= i; k++)
        {
            char check[100];
            if (commands[k] != NULL)
            {
                strcpy(check, commands[k]);
                char *token[10];
                token[0] = strtok(check, "&");
                ll l = 0;
                while (token[l++] != NULL)
                {
                    token[l] = strtok(NULL, "&");
                }
                l--;
                for (ll looping = 0; looping < l; looping++)
                {
                    char *command = strtok(token[looping], "\t\n ");
                    if (command != NULL)
                        no_commands++;
                }
            }
        }
        if (no_commands)
        {
            for (ll k = len - 1; k >= 0; k--)
            {
                if (b[k] == ';')
                    no_semicolons--;
                if (b[k] == '&')
                    no_ands--;
                if (b[k] != ' ')
                    break;
            }
        }

        ll j = 0;
        if (no_commands == no_semicolons + no_ands + 1 || (no_semicolons == 0 && no_ands == 0))
        {
            while (j <= i)
            {

                char *token[10];
                ll l = 0;
                int flag = 0;
                if (commands[j][strlen(commands[j]) - 1] == '&')
                    flag = 1;
                token[0] = strtok(commands[j], "&");

                while (token[l++] != NULL)
                {
                    token[l] = strtok(NULL, "&");
                }
                l--;
                if (flag == 1)
                    l++;
                ll looping = 0;
                // printf("%s\n", command);

                while (looping < l - 1)
                {
                    no_of_backgrounds++;
                    char *command;
                    int original, originalread;
                    int redirectfd, redirectedfdread;
                    int mask;
                    char b[100] = {'\0'}, c[100] = {'\0'}, d[100] = {'\0'};
                    strcpy(b, token[looping]);
                    strcpy(c, token[looping]);
                    strcpy(d, token[looping]);

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
                            printf("File couldn't open\n");
                            looping++;
                            continue;
                        }
                        if ((original = dup(STDIN_FILENO)) < 0)
                        {
                            printf("dup failed\n");
                            looping++;
                            continue;
                        }
                        if (dup2(redirectfd, STDOUT_FILENO) < 0)
                        {
                            printf("dup failed\n");
                            looping++;
                            continue;
                        }
                    }

                    char *redirectread = strtok(d, "<");
                    redirectread = strtok(NULL, "<");
                    if (redirectread != NULL)
                    {
                        isredirectedread = 1;
                        redirectread = strtok(redirectread, "> ");
                        redirectedfdread = open(redirectread, O_RDONLY);
                        if (errno == ENOENT)
                        {
                            printf("Invalid file name\n");
                            looping++;
                            continue;
                        }
                        if (redirectedfdread < 0)
                        {
                            printf("File couldn't open\n");
                            looping++;
                            continue;
                        }
                        if ((originalread = dup(STDIN_FILENO)) < 0)
                        {
                            printf("dup failed\n");
                            looping++;
                            continue;
                        }
                        if (dup2(redirectedfdread, STDIN_FILENO) < 0)
                        {
                            printf("dup failed\n");
                            looping++;
                            continue;
                        }
                    }
                    command = strtok(b, ">>");
                    command = strtok(b, ">");
                    command = strtok(b, "<");
                    command = strtok(token[looping], ">>");
                    command = strtok(token[looping], ">");
                    command = strtok(token[looping], "<");
                    command = strtok(token[looping], "\t\n ");
                    // printf("%s\n", command);

                    if (command != NULL)
                    {
                        if (!strcmp("cd", command))
                        {
                            docd(home_directory, old_directory);
                        }
                        else if (!strcmp("pwd", command))
                        {
                            dopwd();
                        }
                        else if (!strcmp("echo", command))
                        {
                            doecho();
                        }
                        else if (!strcmp("ls", command))
                        {
                            dols(home_directory);
                        }
                        else if (!strcmp("discover", command))
                        {
                            dodiscovery(home_directory);
                        }
                        else if (!strcmp("pinfo", command))
                        {
                            dopinfo();
                        }
                        else if (!strcmp("history", command))
                        {
                            dohistory(home_directory);
                        }
                        else
                        {
                            background(b, no_of_backgrounds);
                        }
                        looping++;
                        if (isredirected)
                        {
                            close(redirectfd);
                            if (dup2(original, STDOUT_FILENO) < 0)
                            {
                                printf("dup failed\n");
                                exit(1);
                            }
                        }
                        if (isredirectedread)
                        {
                            close(redirectedfdread);
                            if (dup2(originalread, STDIN_FILENO) < 0)
                            {
                                printf("dup failed\n");
                                exit(1);
                            }
                        }
                    }
                }
                if (!flag)
                {
                    char *command;
                    char b[100] = {'\0'}, c[100] = {'\0'}, d[100] = {'\0'};
                    int original, originalread;
                    int redirectfd, redirectedfdread;
                    int mask;
                    strcpy(b, token[l - 1]);
                    strcpy(c, token[l - 1]);
                    strcpy(d, token[l - 1]);
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
                            printf("File couldn't open\n");
                            j++;
                            continue;
                        }
                        if ((original = dup(STDIN_FILENO)) < 0)
                        {
                            printf("dup failed\n");
                            j++;
                            continue;
                        }
                        if (dup2(redirectfd, STDOUT_FILENO) < 0)
                        {
                            printf("dup failed\n");
                            j++;
                            continue;
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
                        if (errno == ENOENT)
                        {
                            printf("Invalid file name\n");
                            j++;
                            continue;
                        }
                        if (redirectedfdread < 0)
                        {
                            printf("File couldn't open\n");
                            j++;
                            continue;
                        }
                        if ((originalread = dup(STDIN_FILENO)) < 0)
                        {
                            printf("dup failed\n");
                            j++;
                            continue;
                        }
                        if (dup2(redirectedfdread, STDIN_FILENO) < 0)
                        {
                            printf("dup failed\n");
                            j++;
                            continue;
                        }
                    }
                    command = strtok(b, ">>");
                    command = strtok(b, ">");
                    command = strtok(b, "<");
                    command = strtok(token[l - 1], ">>");
                    command = strtok(token[l - 1], ">");
                    command = strtok(token[l - 1], "<");
                    command = strtok(token[l - 1], "\t\n ");
                    // printf("%s\n", command);
                    if (command != NULL)
                    {
                        if (!strcmp("cd", command))
                        {
                            docd(home_directory, old_directory);
                        }
                        else if (!strcmp("pwd", command))
                        {
                            dopwd();
                        }
                        else if (!strcmp("echo", command))
                        {
                            doecho();
                        }
                        else if (!strcmp("ls", command))
                        {
                            dols(home_directory);
                        }
                        else if (!strcmp("discover", command))
                        {
                            dodiscovery(home_directory);
                        }
                        else if (!strcmp("pinfo", command))
                        {
                            dopinfo();
                        }
                        else if (!strcmp("history", command))
                        {
                            dohistory(home_directory);
                        }
                        else
                        {
                            time = foreground(b);
                            if (time < 1)
                                time = -1;
                        }
                        if (isredirected)
                        {
                            close(redirectfd);

                            if (dup2(original, STDOUT_FILENO) < 0)
                            {
                                printf("dup failed\n");
                                exit(1);
                            }
                            close(original);
                        }
                        if (isredirectedread)
                        {
                            if (dup2(originalread, STDIN_FILENO) < 0)
                            {
                                printf("dup failed\n");
                                exit(1);
                            }
                            close(redirectedfdread);
                            close(originalread);
                        }
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
