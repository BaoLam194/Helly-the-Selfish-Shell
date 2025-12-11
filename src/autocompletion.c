#include "helper.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>

const char *BUILT_IN_COMMAND[] = {"echo", "pwd", "cd", "exit", "type"};
const int BUILT_IN_SIZE = sizeof(BUILT_IN_COMMAND) / sizeof(BUILT_IN_COMMAND[0]);
// This to reduce duplicates and unnecessary command
// Return string array with NULL terminated
char **list_executable_in_path(const char *text, int len) {
  char *path, *saveptr;
  char *paths = strdup(getenv("PATH"));
  //"/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/bin"
  if (!paths)
    return NULL;
  path = strtok_r(paths, PATH_LIST_SEPARATOR, &saveptr);
  char **result = malloc(sizeof(char *) * MAX_ARGUMENT_COUNT);
  int count = 0;
  while (path != NULL) { // Check for each seperate directory
    DIR *directory;
    struct dirent *entry;
    directory = opendir(path);
    if (directory == NULL) { // Can not open directory in path ? -> problem!!!, just skip : )
      path = strtok_r(NULL, PATH_LIST_SEPARATOR, &saveptr); // otherwise it get stuck :/
      continue;
    }

    while ((entry = readdir(directory)) != NULL) {
      if (strncmp(text, entry->d_name, len) == 0) {
        char temp[MAX_ARGUMENT_LENGTH];
        snprintf(temp, MAX_ARGUMENT_LENGTH, "%s/%s", path, entry->d_name);
        if (access(temp, F_OK | X_OK) == 0) {
          result[count++] = strdup(entry->d_name);
        }
      }
    }
    closedir(directory);
    path = strtok_r(NULL, PATH_LIST_SEPARATOR, &saveptr);
  }
  result[count] = NULL;
  return result;
}

char *command_generator(const char *text, int state) {
  static int in_index, len, ex_index;
  static char **executable_file;
  if (state == 0) {
    in_index = 0;
    ex_index = 0;
    len = strlen(text);
    executable_file = list_executable_in_path(text, len);
  }
  // This is to handle built in command
  while (in_index < BUILT_IN_SIZE) {
    const char *name = BUILT_IN_COMMAND[in_index++];
    if (strncmp(name, text, len) == 0) {
      return strdup(name); // the rl_completion_matches handle the remove;
    }
  }
  // This one is for executable in path

  char *temp;
  while ((temp = executable_file[ex_index++]) != NULL) {
    return strdup(temp);
  }

  // When it comes to here, there is no need to do anything with the storage, so release the memory
  for (int i = 0; i < ex_index; i++) {
    free(executable_file[i]);
  }
  free(executable_file);
  return NULL;
}

char **command_completion(const char *text, int start, int end) {
  return rl_completion_matches(text, command_generator);
}