#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>

void DieWithError(const char *errorMessage) {
    perror(errorMessage);
    exit(1);
}

