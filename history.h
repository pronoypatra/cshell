#ifndef HISTORY_H
#define HISTORY_H

#include "helper.h"

#define HISTORY 20

typedef long long int ll;

void dohistory(char * home_directory);
int gethistoryfirst(char history[HISTORY][MAX_COMMAND_LENGTH], int * numofcommands, int fd);
void gethistory(char history[HISTORY][MAX_COMMAND_LENGTH], char *historypath, char * a, int* historyindex, int*numofcommands);

#endif