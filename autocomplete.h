#ifndef AUTOCOMPLETE_H
#define AUTOCOMPLETE_H

typedef long long int ll;

void disableRawMode();
void enableRawMode();
void die(const char *s);
int autocomplete(char *a, char* home_directory, int pt);

#endif