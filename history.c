#include "history.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int gethistoryfirst(char history[HISTORY][MAX_COMMAND_LENGTH], int *numofcommands, int fd)
{
    int historyindex = 0;
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
                    *numofcommands = *numofcommands + 1;
                }
            }
        }
    }
    return historyindex;
}

void gethistory(char history[HISTORY][MAX_COMMAND_LENGTH], char *historypath, char *a, int* historyindex, int* numofcommands)
{
    if (*numofcommands < HISTORY)
    {
        if (strcmp(a, history[*historyindex - 1]))
        {
            int fd = open(historypath, O_RDWR | O_TRUNC);

            strcpy(history[*historyindex], a);
            history[*historyindex][strlen(history[*historyindex])] = '\0';
            *historyindex = (*historyindex + 1) % HISTORY;
            *numofcommands = *numofcommands + 1;
            for (int i = 0; i < *numofcommands; i++)
            {

                char buf[102];
                strcpy(buf, history[i]);
                buf[strlen(history[i])] = '\n';
                buf[strlen(history[i]) + 1] = '\0';
                write(fd, buf, strlen(buf));
            }
            close(fd);
        }
    }
    else
    {
        if (strcmp(history[((*historyindex - 1) % HISTORY + HISTORY) % HISTORY], a))
        {
            int fd = open(historypath, O_RDWR | O_TRUNC);

            strcpy(history[*historyindex], a);
            history[*historyindex][strlen(history[*historyindex])] = '\0';

            *historyindex = (*historyindex + 1) % HISTORY;
            int j = *historyindex;
            for (int i = 0; i < HISTORY; i++)
            {
                char buf[102];
                strcpy(buf, history[j]);
                buf[strlen(history[j])] = '\n';
                buf[strlen(history[j]) + 1] = '\0';
                write(fd, buf, strlen(buf));
                j = (j + 1) % HISTORY;
            }
            close(fd);
        }
    }
}

void dohistory(char *home_directory)
{
    char historypath[MAX_PATH_LENGTH];
    sprintf(historypath, "%s/%s", home_directory, "history.txt");
    int fd = open(historypath, O_RDWR);
    char history[HISTORY][MAX_COMMAND_LENGTH];
    int historyindex = 0;
    int validread = 1;
    while (validread > 0)
    {
        char buf[2000] = {'\0'};
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
                    historyindex = (historyindex + 1);
                }
            }
        }
    }
    if (historyindex == 0)
    {
        printf("No history yet\n");
    }
    else
    {
        if (historyindex <= 10)
        {
            for (int i = 0; i < historyindex; i++)
            {
                printf("%s\n", history[i]);
            }
        }
        else
        {
            for (int i = historyindex - 10; i < historyindex; i++)
            {
                printf("%s\n", history[i]);
            }
        }
    }
    close(fd);
}