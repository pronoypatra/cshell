#include "helper.h"
#include "history.h"
#include "builtin.h"
#include "discovery.h"
#include "pinfo.h"
#include "ls.h"
#include "jobs.h"
#include "fgbg.h"
#include <string.h>

ll tokenize(char *s, char **commands, char *delimiter)
{
    commands[0] = strtok(s, delimiter);

    ll i = 0;
    // printf("%lld\n", i);
    // printf("%s\n", commands[0]);

    while (commands[i++] != NULL)
    {
        commands[i] = strtok(NULL, delimiter);
    }

    i -= 2;

    return i;
}

int issyntaxerror(char *b, char **commands, int i)
{
    ll len = strlen(b);
    int no_semicolons = 0, no_ands = 0, no_commands = 0;
    for (ll k = 0; k < len; k++)
    {
        if (b[k] == ';')
            no_semicolons++;
        else if (b[k] == '&')
            no_ands++;
    }
    for (ll k = 0; k <= i; k++)
    {
        char check[100];
        if (commands[k] != NULL)
        {
            strcpy(check, commands[k]);
            char *token[10];

            ll l = tokenize(check, token, "&");
            for (ll looping = 0; looping <= l; looping++)
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
    return (no_commands == no_semicolons + no_ands + 1 || (no_semicolons == 0 && no_ands == 0));
}

int whichcommand(char *command, char *home_directory, char *old_directory)
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
    else if (!strcmp("jobs", command))
    {
        dojobs();
    }
    else if(!strcmp("fg", command))
    {
        fg();
    }
    else if(!strcmp("bg", command))
    {
        bg();
    }
    else if(!strcmp("sig", command))
    {
        sig();
    }
    else if(!strcmp("exit", command))
    {
        doexit();
    }
    else if(!strcmp("clear", command))
    {
        doclear();
    }
    else
    {
        return 0;
    }
    return 1;
}