#include "autocomplete.h"
#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include "ls.h"
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include "prompt.h"

void die(const char *s)
{
    perror(s);
    exit(1);
}

struct termios orig_termios;

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

int issubstring(char *a, char *b)
{
    ll lena = strlen(a);
    ll lenb = strlen(b);
    if (lenb < lena)
        return 0;
    for (ll i = 0; i < lena; i++)
    {
        if (a[i] != b[i])
            return 0;
    }
    return 1;
}

int autocomplete(char *a, char *home_directory, int pt)
{
    char b[100];
    strcpy(b, a);
    char *parameter, lastparameter[100];
    for (ll i = pt - 1; i >= 0; i--)
    {
        if (a[i] == ' ')
        {
            strcpy(lastparameter, &a[i + 1]);
            break;
        }
    }
    char directory[200], temp[100];
    ll nameslash = 0;
    for (nameslash = strlen(lastparameter) - 1; nameslash >= 0; nameslash--)
    {
        if (lastparameter[nameslash] == '/')
            break;
    }
    for (ll i = 0; i < nameslash; i++)
    {
        temp[i] = lastparameter[i];
        if (i == nameslash - 1)
            temp[i + 1] = '\0';
    }
    if (nameslash < 0)
    {
        directory[0] = '.';
        directory[1] = '\0';
    }
    else if (lastparameter[0] == '~')
        sprintf(directory, "%s%s", home_directory, &temp[1]);
    else
        sprintf(directory, "%s", temp);
    // while (parameter != NULL)
    // {
    //     strcpy(lastparameter, parameter);
    //     lastparameter[strlen(parameter)] = '\0';
    //     parameter = strtok(NULL, " \t\n");
    // }
    struct dirent **dirents;
    int n = scandir(directory, &dirents, NULL, NULL);
    char eligible[n][100];
    ll eligiblecount = 0;

    for (ll i = 0; i < n; i++)
    {
        if (issubstring(&lastparameter[nameslash + 1], dirents[i]->d_name))
        {
            strcpy(eligible[eligiblecount], dirents[i]->d_name);

            struct stat x;
            stat(dirents[i]->d_name, &x);

            if (S_ISREG(x.st_mode))
                eligible[eligiblecount][strlen(eligible[eligiblecount])] = ' ';
            else if (S_ISDIR(x.st_mode))
                eligible[eligiblecount][strlen(eligible[eligiblecount])] = '/';
            eligible[eligiblecount][strlen(dirents[i]->d_name) + 1] = '\0';
            eligiblecount++;
        }
    }
    printf("\33[2K");
    printf("\r");
    char printstring[200];
    int minlen = 200;
    int mini = 0;
    for (ll i = 0; i < eligiblecount; i++)
    {
        if (eligiblecount != 1)
            printf("%s\n", eligible[i]);
        if (strlen(eligible[i]) < minlen)
        {
            minlen = strlen(eligible[i]);
            mini = i;
        }
    }
    ll maxlength = minlen;
    int flag = 0;

    for (ll j = 0; j < minlen; j++)
    {
        if (flag)
            break;
        for (ll i = 0; i < eligiblecount; i++)
        {
            if (eligible[i][j] != eligible[mini][j])
            {
                maxlength = j;
                flag = 1;
                break;
            }
        }
    }

    char current_directory[100];
    getcwd(current_directory, sizeof(current_directory));
    prompt(home_directory, current_directory, -1);
    char substring[100];
    strncpy(substring, eligible[mini], maxlength);
    substring[maxlength] = '\0';
    // if (eligiblecount != 0)
    //     sprintf(printstring, "%s%s", a, &substring[strlen(&lastparameter[nameslash + 1])]);
    // else
    //     sprintf(printstring, "%s", a);
    // for (ll i = strlen(a); i < 100; i++)
    // {
    //     a[i] = printstring[i];
    // }
    int c = 0;
    ll breakpoint = strlen(a);
    if (eligiblecount != 0)
    {
        for (ll i = 0; i < strlen(a); i++)
        {
            if (issubstring(&a[i], substring))
            {
                breakpoint = i;
                break;
            }
        }
        for (ll i = 0; i < strlen(substring); i++)
        {
            a[i + breakpoint] = substring[i];
            if (i == strlen(substring) - 1)
                a[i + breakpoint+ 1] = '\0';
        }
    }

    printf("%s", a);
    if (eligiblecount != 0)
        return strlen(substring) - strlen(a) + breakpoint ;
    else
        return (0);
}