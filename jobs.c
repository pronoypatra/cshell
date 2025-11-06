#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "jobs.h"
#include "helper.h"

extern ll number[MAX_BACKGROUND_JOBS];
extern char backcommand[MAX_BACKGROUND_JOBS][MAX_COMMAND_LENGTH];
extern int backgroundpid[MAX_BACKGROUND_JOBS];

ll sortednumber[MAX_BACKGROUND_JOBS];
char sortedbackcommand[MAX_BACKGROUND_JOBS][MAX_COMMAND_LENGTH];
int sortedbackgroundpid[MAX_BACKGROUND_JOBS];