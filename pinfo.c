#include "pinfo.h"
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

void dopinfo()
{
    char *pid = strtok(NULL, " ");
    char procstat[100] = "/proc/";
    char procpath[100] = "/proc/";
    if (pid == NULL)
    {
        int id = getpid();
        char cpid[6];
        sprintf(cpid, "%d", id);
        strcat(procstat, cpid);
        strcat(procpath, "self");
    }
    else
    {
        strcat(procstat, pid);
        strcat(procpath, pid);
    }
    strcat(procstat, "/stat");
    strcat(procpath, "/exe");
    int fd = open(procstat, O_RDONLY);
    if (fd > 0)
    {
        char buf[200];
        read(fd, buf, 200);
        char *x = strtok(buf, " ");
        close(fd);
        printf("pid: %s\n", x);
        char a[2], b[10], c[10], d[20];
        a[1] = '\0';
        b[9] = '\0';
        c[9] = '\0';
        d[19] = '\0';
        for (int i = 2; i <= 23; i++)
        {
            x = strtok(NULL, " ");
            if (i == 5)
                strcpy(b, x);
            else if (i == 8)
                strcpy(c, x);
            else if (i == 3)
                strcpy(a, x);
            else if (i == 23)
                strcpy(d, x);
        }
        if (strcmp(b, c))
        {
            printf("processs Stat: %s\n", a);
        }
        else
        {
            printf("processs Stat: %s+\n", a);
        }
        printf("memory : %s\n", d);
        char path[100];
        int size = readlink(procpath, path, sizeof(path));
        path[size] = '\0';
        if (size == -1)
            printf("%s\n", procpath);
        else
        {
            printf("executable Path: %s\n", path);
        }
    }
    else
    {
        printf("Invalid Pid\n");
    }
}