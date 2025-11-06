#include "discovery.h"
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

int dfs(char *directory_info, char *print_directory, ll file_name_given, char *filename, ll file, ll directory)
{
    if (file_name_given)
    {
        struct dirent **dirents;
        errno = 0;
        ll n = scandir(directory_info, &dirents, NULL, alphasort);
        if (!errno)
        {
            for (ll i = 0; i < n; i++)
            {

                if (dirents[i]->d_name[0] != '.')
                {
                    char directory_info2[100] = {'\0'};
                    char print_directory2[100] = {'\0'};
                    strcpy(directory_info2, directory_info);
                    strcpy(print_directory2, print_directory);
                    errno = 0;
                    directory_info2[strlen(directory_info2)] = '/';
                    print_directory2[strlen(print_directory2)] = '/';

                    for (ll j = 0; j < strlen(dirents[i]->d_name); j++)
                    {
                        directory_info2[strlen(directory_info2)] = dirents[i]->d_name[j];
                        print_directory2[strlen(print_directory2)] = dirents[i]->d_name[j];
                    }
                    directory_info2[strlen(directory_info2)] = '\0';
                    print_directory2[strlen(print_directory2)] = '\0';
                    if (!strcmp(filename, dirents[i]->d_name))
                    {
                        printf("%s/%s\n", print_directory, filename);
                        return 1;
                    }
                    int x = dfs(directory_info2, print_directory2, file_name_given, filename, file, directory);
                    if (x == 1)
                        return 1;
                }
            }
        }
        else if (errno == ENOENT)
        {
            printf("No file or directory\n");
        }
        errno = 0;
        return 0;
    }
    else
    {
        struct dirent **dirents;
        errno = 0;
        ll n = scandir(directory_info, &dirents, NULL, alphasort);

        if (!errno)
        {
            for (ll i = 0; i < n; i++)
            {

                if (dirents[i]->d_name[0] != '.')
                {
                    char directory_info2[100] = {'\0'};
                    char print_directory2[100] = {'\0'};
                    strcpy(directory_info2, directory_info);
                    strcpy(print_directory2, print_directory);
                    errno = 0;
                    directory_info2[strlen(directory_info2)] = '/';
                    print_directory2[strlen(print_directory2)] = '/';

                    for (ll j = 0; j < strlen(dirents[i]->d_name); j++)
                    {
                        directory_info2[strlen(directory_info2)] = dirents[i]->d_name[j];
                        print_directory2[strlen(print_directory2)] = dirents[i]->d_name[j];
                    }
                    directory_info2[strlen(directory_info2)] = '\0';
                    print_directory2[strlen(print_directory2)] = '\0';
                    if (!strcmp(filename, dirents[i]->d_name))
                    {
                        printf("%s/%s\n", print_directory, filename);
                        return 1;
                    }
                    struct stat x;
                    stat(directory_info2, &x);
                    char *color[4] = {"\e[m", "\033[1;37m", "\033[1;34m", "\033[1;32m"};

                    if (S_ISDIR(x.st_mode) && directory)
                    {
                        printf("%s", color[2]);
                        printf("%s\n", print_directory2);
                        printf("%s", color[0]);
                    }
                    if (!S_ISDIR(x.st_mode) && file)
                    {
                        if (x.st_mode & S_IXUSR)
                            printf("%s", color[3]);
                        printf("%s\n", print_directory2);
                        if (x.st_mode & S_IXUSR)
                            printf("%s", color[0]);
                    }
                    dfs(directory_info2, print_directory2, file_name_given, filename, file, directory);
                }
            }
        }
        else if (errno == ENOENT)
        {
            printf("No file or directory\n");
        }
        errno = 0;
        return 0;
    }
}

void dodiscovery(char *home_directory)
{
    struct dirent **dirents;
    ll directory = 0;
    ll file = 0;
    ll scanned = 0;
    ll file_name_given = 0;
    char *parameter;
    char filename[200] = {'\0'};
    char directory_info[100] = {'\0'};
    char print_directory[100] = {'\0'};
    parameter = strtok(NULL, " ");
    while (parameter != NULL)
    {

        if (parameter[0] == '-')
        {
            for (ll k = 0; k < strlen(parameter); k++)
            {
                if (parameter[k] == 'd')
                    directory = 1;
                else if (parameter[k] == 'f')
                    file = 1;
            }
        }
        else
        {
            if (parameter[0] == '~' || parameter[1] == '~')
            {

                char parameter2[200];
                sprintf(parameter2, "%s%s", home_directory, &parameter[1]);
                errno = 0;

                if (parameter[0] == '"' && parameter[strlen(parameter) - 1] == '"')
                {
                    file_name_given = 1;
                    strcpy(filename, &parameter[3]);
                    filename[strlen(filename) - 1] = '\0';
                    // printf("%s\n", filename);
                }
                else
                {

                    strcpy(directory_info, parameter2);
                    directory_info[strlen(parameter2)] = '\0';
                    strcpy(print_directory, parameter);
                    print_directory[strlen(parameter)] = '\0';
                    scanned = 1;
                }
            }
            else
            {
                errno = 0;

                if (parameter[0] == '"' && parameter[strlen(parameter) - 1] == '"')
                {
                    file_name_given = 1;
                    strcpy(filename, &parameter[1]);
                    filename[strlen(filename) - 1] = '\0';
                    // printf("%s\n", filename);
                }
                else
                {

                    scanned = 1;
                    if (parameter[0] == '.' || parameter[0] == '/')
                    {
                        strcpy(directory_info, parameter);
                        directory_info[strlen(parameter)] = '\0';
                        strcpy(print_directory, parameter);
                        print_directory[strlen(parameter)] = '\0';
                    }
                    else
                    {
                        getcwd(directory_info, sizeof(directory_info));
                        directory_info[strlen(directory_info)] = '/';
                        print_directory[0] = '.';
                        print_directory[1] = '/';

                        for (ll i = 0; i < strlen(parameter); i++)
                        {
                            directory_info[strlen(directory_info)] = parameter[i];
                            print_directory[i + 2] = parameter[i];
                        }
                        directory_info[strlen(directory_info)] = '\0';
                        print_directory[strlen(print_directory)] = '\0';
                    }
                }
            }
        }
        parameter = strtok(NULL, " ");
    }
    if (directory == 0 && file == 0)
    {
        directory = 1;
        file = 1;
    }
    if (scanned)
    {

        if (!dfs(directory_info, print_directory, file_name_given, filename, file, directory) && file_name_given)
            printf("File doesn't exist\n");
    }
    else
    {
        getcwd(directory_info, sizeof(directory_info));
        strcpy(print_directory, ".\0");
        if (!file_name_given)
            printf("%s\n", print_directory);
        if (!dfs(directory_info, print_directory, file_name_given, filename, file, directory) && file_name_given)
            printf("File doesn't exist\n");
    }
}