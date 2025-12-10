#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGUMENT_COUNT 100
#define MAX_ARGUMENT_LENGTH 1024

// Command to execute
bool is_built_in(char *command);
void execute_built_in(char **command, int count, char **cwd);
void execute_existing(char **command, int count);
char **parse_input(char *input, int *count);
typedef enum { // use as
  NORMAL = 1,
  SINGLE_QUOTE = 2,
  DOUBLE_QUOTE = 3,
} state;

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

// To support pwd, cd command
#include <sys/stat.h>
bool check_path_to_dir(char *path, char *from, char **dest);
int my_max(int a, int b);
