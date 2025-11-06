#ifndef LS_H
#define LS_H

#include <dirent.h>


typedef long long int ll;

void printfile(char *filename, ll hidden, ll format);
void printdirectory(struct dirent **dirents, ll hidden, ll format, ll n, char *directory_info);
void dols(char *home_directory);


#endif