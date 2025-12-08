#include "helper.h"
char *check_executable_file_in_path(const char *file) {
  // Get path variable and split
  char *split, *savepath;
  char *path = strdup(getenv("PATH"));
  if (!path)
    return NULL;
  split = strtok_r(path, PATH_LIST_SEPARATOR, &savepath);
  while (split != NULL) { // Check for each seperate directory
    char temp[500] = "";
    strcat(temp, split);
    strcat(temp, "/");
    strcat(temp, file);
    if (access(temp, F_OK | X_OK) == 0) {
      free(path);
      char *result = strdup(temp); // Allocate memory for return T_T
      return result;
    }
    split = strtok_r(NULL, PATH_LIST_SEPARATOR, &savepath);
  }

  free(path);
  return NULL; // CANNOT find the file or no executable permission
}

bool check_path_to_dir(char *path, char *from, char **dest) {
  if (path[0] == '/' || path[0] == '~') { // Absolute path

    char *copy_path;
    if (path[0] == '~') {
      char *home = strdup(getenv("HOME"));
      if (!home)
        return false;
      copy_path = malloc(sizeof(char) * MAX_PATH_LENGTH);
      copy_path[0] = '\0';
      strcat(copy_path, home);
      strcat(copy_path, path + 1);
      free(home);
    }
    else {
      copy_path = strdup(path);
    }
    if (access(copy_path, X_OK) != 0) {
      // can not access (could be no execute permission
      // or path not exist)
      free(copy_path);
      return false;
    }
    struct stat sb;
    if (stat(copy_path, &sb) == 0 && S_ISDIR(sb.st_mode)) {
      // path exists and is directory
      if (path[0] == '~') {
        *dest = strdup(copy_path);
      }
      free(copy_path);
      return true;
    }
    *dest = NULL;
    free(copy_path);
    // else wrong
    return false;
  }
  else {
    // Tokenize the path and cwd into array
    char *path_split_1[MAX_ARGUMENT_COUNT];
    int count_1 = 0;
    char *saveptr_1, *saveptr_2;
    char *dup_input_1 = strdup(path), *dup_input_2 = strdup(from);
    char *token = strtok_r(dup_input_1, "/", &saveptr_1);
    while (token != NULL) {
      path_split_1[count_1++] = strdup(token);
      if (count_1 >= MAX_ARGUMENT_COUNT) {
        perror("More than 100 argumeants!!! What are you doing?");
        exit(1);
      }
      token = strtok_r(NULL, "/", &saveptr_1);
    }
    char *path_split_2[MAX_ARGUMENT_COUNT];
    int count_2 = 0;
    bool is_absolute = false;
    if (strncmp(from, "/", 1) == 0) {
      is_absolute = true;
    }
    token = strtok_r(dup_input_2, "/", &saveptr_2);
    while (token != NULL) {
      path_split_2[count_2++] = strdup(token);
      if (count_2 >= MAX_ARGUMENT_COUNT) {
        perror("More than 100 argumeants!!! What are you doing?");
        exit(1);
      }
      token = strtok_r(NULL, "/", &saveptr_2);
    }
    for (int i = 0; i < count_1; i++) { // build dest from path and from
      if (strcmp(path_split_1[i], ".") == 0)
        continue;
      if (strcmp(path_split_1[i], "..") == 0) {
        count_2--;
        if (count_2 < 0) { // unable to go lower
          free(dup_input_1);
          free(dup_input_2);
          for (int i = 0; i < count_1; i++) {
            free(path_split_1[i]);
          }
          for (int i = 0; i < count_2; i++) {
            free(path_split_2[i]);
          }
          return false;
        }
        free(path_split_2[count_2]);
      }
      else {
        path_split_2[count_2++] = strdup(path_split_1[i]);
        if (count_2 >= MAX_ARGUMENT_COUNT) {
          perror("More than 100 argumeants!!! What are you doing?");
          exit(1);
        }
      }
    }

    // Allocate memory and construct the new destination
    *dest = malloc(sizeof(char) * MAX_PATH_LENGTH);
    if (is_absolute) {
      (*dest)[0] = '/';
      (*dest)[1] = '\0';
    }
    else {
      (*dest)[0] = '\0';
    }
    for (int i = 0; i < count_2; i++) {
      strcat(*dest, path_split_2[i]);
      if (i != count_2 - 1)
        strcat(*dest, "/");
    }
    free(dup_input_1);
    free(dup_input_2);
    for (int i = 0; i < count_1; i++) {
      free(path_split_1[i]);
    }
    for (int i = 0; i < count_2; i++) {
      free(path_split_2[i]);
    }

    // check if the new destination is valid
    if (access(*dest, X_OK) != 0) {
      // can not access (could be no execute permission
      // or path not exist)
      free(*dest);
      *dest = NULL;
      return false;
    }
    struct stat sb;
    if (stat(*dest, &sb) == 0 && S_ISDIR(sb.st_mode)) {
      // path exists and is directory
      return true;
    }
    free(*dest);
    *dest = NULL;
    // else wrong
    return false;
  }
  return false;
}
int my_max(int a, int b) { return (a > b) ? a : b; }