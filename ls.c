#include <stdio.h>
#include "ls.h"
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>

void printfile(char *filename, ll hidden, ll format)
{

    if (filename[0] != '.' || hidden == 1 || strlen(filename) > 2)
    {
        struct stat x;
        stat(filename, &x);
        errno = 0;
        char a[11];
        a[10] = '\0';
        for (ll i = 0; i < 10; i++)
        {
            a[i] = '-';
        }
        if (S_ISDIR(x.st_mode))
            a[0] = 'd';
        if (x.st_mode & S_IRUSR)
            a[1] = 'r';
        if (x.st_mode & S_IWUSR)
            a[2] = 'w';
        if (x.st_mode & S_IXUSR)
            a[3] = 'x';
        if (x.st_mode & S_IRGRP)
            a[4] = 'r';
        if (x.st_mode & S_IWGRP)
            a[5] = 'w';
        if (x.st_mode & S_IXGRP)
            a[6] = 'x';
        if (x.st_mode & S_IROTH)
            a[7] = 'r';
        if (x.st_mode & S_IWOTH)
            a[8] = 'w';
        if (x.st_mode & S_IXOTH)
            a[9] = 'x';
        struct tm *time;
        time = localtime(&x.st_mtime);
        char *month[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
        char *color[4] = {"\e[m", "\033[1;37m", "\033[1;34m", "\033[1;32m"};
        if (format == 1)
            printf("%s %ld %s %s %7ld %s %2d %2d:%2d ", a, x.st_nlink, getpwuid(x.st_uid)->pw_name, getgrgid(x.st_gid)->gr_name, x.st_size, month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);

        if (a[3] == 'x' || a[6] == 'x' || a[9] == 'x')
        {
            printf("%s", color[3]);
            printf("%s\n", filename);
            printf("%s", color[0]);
        }
        else
        {
            printf("%s\n", filename);
        }
    }
}

void printdirectory(struct dirent **dirents, ll hidden, ll format, ll n, char *directory_info)
{
    ll total = 0;
    for (ll i = 0; i < n; i++)
    {
        struct stat y;
        char file_path[300] = {'\0'};
        sprintf(file_path, "%s/%s", directory_info, dirents[i]->d_name);
        stat(file_path, &y);
        total += y.st_blocks;
    }
    errno = 0;
    if (format)
        printf("Total: %lld\n", total / 2);
    for (ll i = 0; i < n; i++)
    {
        if (dirents[i]->d_name[0] == '.' && hidden == 0)
            continue;
        struct stat x;
        char file_path[300] = {'\0'};
        sprintf(file_path, "%s/%s", directory_info, dirents[i]->d_name);
        stat(file_path, &x);
        errno = 0;
        char a[11];
        a[10] = '\0';
        for (ll i = 0; i < 10; i++)
        {
            a[i] = '-';
        }
        if (S_ISDIR(x.st_mode))
            a[0] = 'd';
        if (x.st_mode & S_IRUSR)
            a[1] = 'r';
        if (x.st_mode & S_IWUSR)
            a[2] = 'w';
        if (x.st_mode & S_IXUSR)
            a[3] = 'x';
        if (x.st_mode & S_IRGRP)
            a[4] = 'r';
        if (x.st_mode & S_IWGRP)
            a[5] = 'w';
        if (x.st_mode & S_IXGRP)
            a[6] = 'x';
        if (x.st_mode & S_IROTH)
            a[7] = 'r';
        if (x.st_mode & S_IWOTH)
            a[8] = 'w';
        if (x.st_mode & S_IXOTH)
            a[9] = 'x';
        struct tm *time;
        time = localtime(&x.st_mtime);
        errno = 0;
        char *month[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
        if (format == 1)
        {

            printf("%s %ld %s %s %7ld %s %2d %2d:%2d ", a, x.st_nlink, getpwuid(x.st_uid)->pw_name, getgrgid(x.st_gid)->gr_name, x.st_size, month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);
        }
        char *color[4] = {"\e[m", "\033[1;37m", "\033[1;34m", "\033[1;32m"};
        if (a[0] == 'd')
        {
            printf("%s", color[2]);
            printf("%s\n", dirents[i]->d_name);
            printf("%s", color[0]);
        }
        else if (a[3] == 'x' || a[6] == 'x' || a[9] == 'x')
        {
            printf("%s", color[3]);
            printf("%s\n", dirents[i]->d_name);
            printf("%s", color[0]);
        }
        else
        {
            printf("%s\n", dirents[i]->d_name);
        }
    }
}

void dols(char *home_directory)
{
    struct dirent **dirents[20];
    ll format = 0;
    ll hidden = 0;
    ll scanned = 0;
    ll number_of_files[20] = {0};
    ll num_dir = 0;
    ll num_file = 0;
    ll file = 0;
    char *parameter;
    char filename[10][200];
    for (ll i = 0; i < 10; i++)
    {
        strcpy(filename[i], "\0");
    }
    parameter = strtok(NULL, " ");
    char directory_info[10][200];
    for (ll i = 0; i < 10; i++)
    {
        strcpy(directory_info[i], "\0");
    }
    while (parameter != NULL)
    {

        if (parameter[0] == '-')
        {
            for (ll k = 0; k < strlen(parameter); k++)
            {
                if (parameter[k] == 'l')
                    format = 1;
                else if (parameter[k] == 'a')
                    hidden = 1;
            }
        }
        else
        {
            if (parameter[0] == '~')
            {

                char parameter2[200];
                sprintf(parameter2, "%s%s", home_directory, &parameter[1]);
                errno = 0;
                number_of_files[num_dir] = scandir(parameter2, &dirents[num_dir], NULL, alphasort);
                if (errno == ENOTDIR)
                {
                    file = 1;
                    strcpy(filename[num_file], parameter2);
                    num_file++;
                }
                else if(errno == ENOENT)
                {
                    scanned = 1;
                    printf("%s doesn't exist\n", parameter);
                }
                else
                {
                    strcpy(directory_info[num_dir], parameter2);
                    num_dir++;
                    scanned = 1;
                }
                errno = 0;
            }
            else
            {
                errno = 0;

                number_of_files[num_dir] = scandir(parameter, &dirents[num_dir], NULL, alphasort);
                if (errno == ENOTDIR)
                {
                    file = 1;
                    strcpy(filename[num_file], parameter);
                    num_file++;
                }
                else if(errno == ENOENT)
                {
                    scanned = 1;
                    printf("%s doesn't exits\n", parameter);
                }
                else
                {
                    scanned = 1;
                    if (!strcmp(parameter, "."))
                    {
                        getcwd(directory_info[num_dir], sizeof(directory_info));
                    }
                    else if (!strcmp(parameter, ".."))
                    {
                        char current_directory[100];
                        getcwd(current_directory, sizeof(current_directory));
                        ll len = strlen(current_directory);
                        ll k;
                        for (k = len - 1; k >= 0; k--)
                        {
                            if (current_directory[k] == '/')
                                break;
                        }
                        for (ll i = 0; i < k; i++)
                            directory_info[num_dir][i] = current_directory[i];
                        directory_info[num_dir][k] = '\0';
                    }
                    else if (parameter[0] == '/')
                    {
                        sprintf(directory_info[num_dir], "%s", parameter);
                    }
                    else
                    {
                        char current_directory[100];
                        getcwd(current_directory, sizeof(current_directory));
                        sprintf(directory_info[num_dir], "%s/%s", current_directory, parameter);
                    }
                    num_dir++;
                }
                errno = 0;
            }
        }
        parameter = strtok(NULL, " ");
    }
    if (file == 1)
    {
        for (ll i = 0; i < num_file; i++)
            printfile(filename[i], hidden, format);
        if (num_dir > 0)
        {
            printf("\n");
        }
    }

    if (scanned)
    {

        for (ll i = 0; i < num_dir; i++)
        {
            if (num_dir > 1)
                printf("%s:\n", directory_info[i]);
            printdirectory(dirents[i], hidden, format, number_of_files[i], directory_info[i]);
            if (num_dir > 1 && i != num_dir - 1)
                printf("\n");
        }
    }
    else if (num_dir + num_file == 0)
    {
        errno = 0;
        number_of_files[0] = scandir(".", &dirents[0], NULL, alphasort);
        getcwd(directory_info[0], sizeof(directory_info[0]));
        printdirectory(dirents[0], hidden, format, number_of_files[0], directory_info[0]);
        errno = 0;
    }
}