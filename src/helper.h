#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// To support the type command
#include <unistd.h> // for access function
#ifdef _WIN32
#define PATH_LIST_SEPARATOR ";"
#else
#define PATH_LIST_SEPARATOR ":"
#endif
char *check_executable_file_in_path(const char *file);

// To support execute programme
#include <sys/types.h> // pid_t
#include <sys/wait.h>  // wait(), waitpid()