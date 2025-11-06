#include "prompt.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

void prompt(char home_directory[], char return_directory[], long int time)
{
    char *color[4] = {"\e[m", "\033[0;37m", "\033[0;34m", "\033[0;32m"};
    char *font[2] = {"\033[0;m", "\033[1;m"};
    char hostname[100];
    int herror = gethostname(hostname, sizeof(hostname));
    if (herror == -1)
    {
        perror("hostname error");
        exit(1);
    }

    char username[100];
    int uerror = getlogin_r(username, sizeof(username));
    if (uerror == -1)
    {
        perror("username error");
        exit(1);
    }
    char current_directory[100];
    getcwd(current_directory, sizeof(current_directory));

    // printf("%s\n", current_directory);
    char print_directory[200];
    int flag = 1;
    if (strlen(home_directory) <= strlen(current_directory))
    {
        ll x = strlen(home_directory);
        for (ll i = 0; i < x; i++)
        {
            if (home_directory[i] != current_directory[i])
                flag = 0;
        }
        if (flag)
        {
            if (!strcmp(home_directory, current_directory))
                strcpy(print_directory, "~");
            else
            {
                char print_directory2[100];
                int curr_len = strlen(current_directory);
                ll i;
                for (i = 0; i < curr_len; i++)
                {
                    if (home_directory[i] != current_directory[i])
                        break;
                }
                strcpy(print_directory2, &current_directory[i]);
                sprintf(print_directory, "~%s", print_directory2);
            }

            strcpy(return_directory, current_directory);
        }
        else
            strcpy(print_directory, current_directory);
        if (time == -1)
            printf("\033[1;32m%s@%s\e[m:\033[1;34m%s\e[m> ", username, hostname, print_directory);
        else
            printf("\033[1;32m%s@%s\e[m:\033[1;34m%s \e[m \033[1;mtook %lds\033[0;m> ", username, hostname, print_directory, time);
    }
    else
    {
        if (time == -1)
            printf("\033[1;32m%s@%s\e[m:\033[1;34m%s\e[m> ", username, hostname, current_directory);
        else
            printf("\033[1;32m%s@%s\e[m:\033[1;34m%s \e[m \033[1;mtook %lds\033[0;m> ", username, hostname, current_directory, time);
    }
}
