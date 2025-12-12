#include "helper.h"
#include <readline/history.h>
#include <readline/readline.h>

int current_offset_for_write = 0;
// For readline and history
void initialize() {
  // Flush after every printf
  setbuf(stdout, NULL);
  rl_attempted_completion_function = command_completion;
  // Tab -> this attempted completion -> completion -> update shell
  // Enable history
  read_my_history(getenv("HISTFILE"));
}

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
      copy_path = malloc(sizeof(char) * MAX_ARGUMENT_LENGTH);
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
        fprintf(stderr, "More than 100 argumeants!!! What are you doing?");
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
        fprintf(stderr, "More than 100 argumeants!!! What are you doing?");
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
          fprintf(stderr, "More than 100 argumeants!!! What are you doing?");
          exit(1);
        }
      }
    }

    // Allocate memory and construct the new destination
    *dest = malloc(sizeof(char) * MAX_ARGUMENT_LENGTH);
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

// Parse the input and store number of arguments into count, count should be 0
char **parse_input(char *input, int *count, int *flag) {
  if (*count != 0) {
    fprintf(stderr, "You give no argument to parse input");
    exit(1);
    return NULL;
  }
  int end = strlen(input);
  state state = NORMAL;
  char **result = malloc(sizeof(char *) * MAX_ARGUMENT_COUNT);
  char token[MAX_ARGUMENT_LENGTH];
  int cur_len = 0;
  bool escaped = false;
  for (int i = 0; i <= end; i++) {
    switch (state) {
    case NORMAL: {
      if (escaped) { // if it was escaped
        if (cur_len >= MAX_ARGUMENT_LENGTH - 1) {
          fprintf(stderr, "Argument too lengthy, you have %d\n", cur_len);
          for (int j = 0; j < *count; j++) {
            free(result[j]);
          }
          free(result);
          return NULL;
        }
        token[cur_len++] = input[i];
        token[cur_len] = '\0';
        escaped = 0;
        continue;
      }
      if (input[i] == '\\') {
        escaped = true;
      }
      else if (i == end || input[i] == ' ' || input[i] == '\t') { // Delimiter
        if (!cur_len)
          break;
        else {
          result[(*count)++] = strdup(token);
          cur_len = 0;
          token[cur_len] = '\0';
        }
      }
      else if (input[i] == '\'')
        state = SINGLE_QUOTE;
      else if (input[i] == '\"')
        state = DOUBLE_QUOTE;

      else { // Normal case
        if (cur_len >= MAX_ARGUMENT_LENGTH - 1) {
          fprintf(stderr, "Argument too lengthy, you have %d\n", cur_len);
          for (int j = 0; j < *count; j++) {
            free(result[j]);
          }
          free(result);
          return NULL;
        }
        if (input[i] == '>') // if redirection appears
          *flag = 1;
        token[cur_len++] = input[i];
        token[cur_len] = '\0';
      }
      break;
    }
    case SINGLE_QUOTE: {
      if (input[i] == '\'') {
        state = NORMAL;
      }
      else {
        if (cur_len >= MAX_ARGUMENT_LENGTH - 1) {
          fprintf(stderr, "Argument too lengthy, you have %d\n", cur_len);
          for (int j = 0; j < *count; j++) {
            free(result[j]);
          }
          free(result);
          return NULL;
        }
        token[cur_len++] = input[i];
        token[cur_len] = '\0';
      }
      break;
    }
    case DOUBLE_QUOTE: {
      if (escaped) { // if it was escaped
        if (cur_len >= MAX_ARGUMENT_LENGTH - 1) {
          fprintf(stderr, "Argument too lengthy, you have %d\n", cur_len);
          for (int j = 0; j < *count; j++) {
            free(result[j]);
          }
          free(result);
          return NULL;
        }
        if (input[i] != '\"' && input[i] != '\\' && input[i] != '$' && input[i] != '`') {
          token[cur_len++] = '\\';
          token[cur_len] = '\0';
        }
        token[cur_len++] = input[i];
        token[cur_len] = '\0';
        escaped = 0;
        continue;
      }
      if (input[i] == '\\') {
        escaped = true;
      }
      else if (input[i] == '\"') {
        state = NORMAL;
      }
      else {
        if (cur_len >= MAX_ARGUMENT_LENGTH - 1) {
          fprintf(stderr, "Argument too lengthy, you have %d\n", cur_len);
          for (int j = 0; j < *count; j++) {
            free(result[j]);
          }
          free(result);
          return NULL;
        }
        token[cur_len++] = input[i];
        token[cur_len] = '\0';
      }
      break;
    }
    default: {
      break;
    }
    }
  }
  if (state != NORMAL) {
    fprintf(stderr, "You illegally leave some special characters alone\n");
    for (int j = 0; j < *count; j++) {
      free(result[j]);
    }
    free(result);
    return NULL;
  }
  return result;
}

// Extract the new command from input without the redirection and set the file
// descriptor accordingly, similarly to brute force ?
char **extract_redirect_from_input(char **input, int input_cnt, int *res_cnt) {
  char **result = malloc(sizeof(char *) * MAX_ARGUMENT_COUNT);
  for (int i = 0; i < input_cnt;) { // handle i seperately
    if (strcmp(input[i], ">") == 0 || strcmp(input[i], "1>") == 0) {
      // truncated -> output
      if (i == input_cnt - 1) {
        return NULL;
      }
      int fd = open(input[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
      dup2(fd, STDOUT_FILENO); // input
      close(fd);
      i += 2;
    }
    else if (strcmp(input[i], ">>") == 0 || strcmp(input[i], "1>>") == 0) {
      // append output
      if (i == input_cnt - 1) {
        return NULL;
      }
      int fd = open(input[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
      dup2(fd, STDOUT_FILENO);
      close(fd);
      i += 2;
    }
    else if (strcmp(input[i], "2>") == 0) {
      // truncated -> error
      if (i == input_cnt - 1) {
        return NULL;
      }
      int fd = open(input[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
      dup2(fd, STDERR_FILENO); // output
      close(fd);
      i += 2;
    }
    else if (strcmp(input[i], "2>>") == 0) {
      // append error
      if (i == input_cnt - 1) {
        return NULL;
      }
      int fd = open(input[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
      dup2(fd, STDERR_FILENO); // output
      close(fd);
      i += 2;
    }
    else {
      result[(*res_cnt)++] = strdup(input[i]);
      i++;
    }
  }
  return result;
}
