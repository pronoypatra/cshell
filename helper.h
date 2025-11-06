#ifndef HELPER_H
#define HELPER_H

typedef long long int ll;

// Constants for array sizes and limits
#define MAX_BACKGROUND_JOBS 100
#define MAX_COMMAND_LENGTH 100
#define MAX_PIPED_COMMANDS 1000
#define MAX_ARGS 20
#define MAX_BUFFER_SIZE 200
#define MAX_PATH_LENGTH 200
#define MAX_DIRS 10

int whichcommand(char * command, char * home_directrory, char * old_directory);
int issyntaxerror(char * b, char ** commands,int i);
ll tokenize(char *s, char **commands, char *delimiter);

#endif