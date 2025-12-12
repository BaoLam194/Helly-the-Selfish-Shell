#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGUMENT_COUNT 100
#define MAX_ARGUMENT_LENGTH 1024
void initialize();
extern int current_offset_for_write;

// Command to execute
#include <fcntl.h> // For open()
// dup2 from unistd
bool is_immutable_to_redirect(char *command);
bool is_built_in(char *command);
void execute_built_in(char **command, int count, char **cwd);
void handle_command(char **command, int count, char **cwd, int flag);
void execute_existing(char **command, int count);
char **parse_input(char *input, int *count, int *flag);
typedef enum { // use as
  NORMAL = 1,
  SINGLE_QUOTE = 2,
  DOUBLE_QUOTE = 3,
} state;
char **extract_redirect_from_input(char **input, int input_cnt, int *res_cnt);

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

// Auto completion
char **command_completion(const char *text, int start, int end); // the list of completion
char *command_generator(const char *text, int state);
extern const char *BUILT_IN_COMMAND[];
extern const int BUILT_IN_SIZE;
#include <dirent.h> // For reading the directory

// History
void read_my_history(char *file, bool flag);
void write_my_history(char *file);
void append_my_history(char *file);
