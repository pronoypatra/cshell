#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "builtin.h"

void docd(char home_directory[100], char old_directory[100])
{

    char *parameter = strtok(NULL, " ");
    char *parameter2 = strtok(NULL, " ");
    if (!parameter2)
    {
        if (parameter == NULL)
        {
            char current_directory2[100];
            getcwd(current_directory2, sizeof(current_directory2));
            if (chdir(home_directory) < 0)
            {
                printf("bash: cd: %s: No such file or directory", parameter);
            }
            else
            {
                old_directory[0] = '\0';
                strcpy(old_directory, current_directory2);
            }
        }
        else if (parameter[0] == '~')
        {
            char parameter2[200];
            char current_directory2[100];
            getcwd(current_directory2, sizeof(current_directory2));
            sprintf(parameter2, "%s%s", home_directory, &parameter[1]);
            if (chdir(parameter2) < 0)
            {
                printf("bash: cd: %s: No such file or directory\n", parameter);
            }
            else
            {
                old_directory[0] = '\0';
                strcpy(old_directory, current_directory2);
            }
        }
        else if (parameter[0] == '-')
        {
            if (!old_directory[0])
                printf("bash: cd: OLDPWD not set\n");
            else
            {
                char current_directory2[100];
                getcwd(current_directory2, sizeof(current_directory2));
                chdir(old_directory);
                printf("%s\n", old_directory);
                old_directory[0] = '\0';
                strcpy(old_directory, current_directory2);
            }
        }
        else
        {
            char current_directory2[100];
            getcwd(current_directory2, sizeof(current_directory2));
            if (chdir(parameter) < 0)
            {
                printf("bash: cd: %s: No such file or directory\n", parameter);
            }
            else
            {
                old_directory[0] = '\0';
                strcpy(old_directory, current_directory2);
            }
        }
    }
    else
    {
        printf("bash: cd: too many arguments\n");
    }
}

void dopwd()
{
    char current_directory[100];
    getcwd(current_directory, sizeof(current_directory));
    printf("%s\n", current_directory);
}

void doecho()
{
    char *parameter = strtok(NULL, " ");
    while (parameter != NULL)
    {
        printf("%s ", parameter);
        parameter = strtok(NULL, " ");
    }
    printf("\n");
}

void doexit()
{
    char *parameter = strtok(NULL, " ");
    if (parameter != NULL)
    {
        int exit_code = atoi(parameter);
        exit(exit_code);
    }
    else
    {
        exit(0);
    }
}

void doclear()
{
    printf("\033[2J\033[H");
    fflush(stdout);
}