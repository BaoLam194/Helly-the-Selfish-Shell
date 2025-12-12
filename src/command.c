#include "helper.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
bool is_built_in(char *command) {
  if (strcmp(command, "type") == 0 || strcmp(command, "exit") == 0 || strcmp(command, "echo") == 0 ||
      strcmp(command, "pwd") == 0 || strcmp(command, "cd") == 0 || strcmp(command, "history") == 0) {
    return true;
  }
  else {
    return false;
  }
}

bool is_immutable_to_redirect(char *command) { // So that we don't need to spawn new process for
  if (strcmp(command, "exit") == 0 || strcmp(command, "cd") == 0) {
    return true;
  }
  else {
    return false;
  }
}
// exit command with history handle : )
void exit_command(int status) {
  write_my_history(getenv("HISTFILE"), false);
  exit(status);
}

// These function behaves with many arguments
// as we execute each argument independently
void echo_command(char *argument) { printf("%s ", argument); }
void type_command(char *argument) {
  if (is_built_in(argument)) {
    printf("%s is a shell builtin", argument);
  }
  else { // if not built in
    char *temp = check_executable_file_in_path(argument);
    if (!temp)
      fprintf(stderr, "%s: not found", argument);
    else {
      printf("%s is %s", argument, temp);
      free(temp); // free the allocated memory
    }
  }
  printf("\n");
}
void history_command(int len, char **command) {
  if (command == NULL) { // Default behaviour,
    HIST_ENTRY **my_his = history_list();
    int start;
    if (len == -1) {
      start = 0;
    }
    else {
      start = history_length - len;
    }
    for (start; my_his[start] != NULL; start++) {
      printf(" %d %s\n", start + 1, my_his[start]->line);
    }
  }
  else {                                 // Flag etc : -n, -r
    if (strcmp(command[1], "-r") == 0) { // read
      read_my_history(command[2], true);
    }
    else if (strcmp(command[1], "-w") == 0) { // write
      write_my_history(command[2], true);
    }
    else if (strcmp(command[1], "-a") == 0) { // append
      append_my_history(command[2], true);
    }
  }
}
// execute
void execute_built_in(char **command, int count, char **cwd) {

  char *command_token = command[0];
  if (strcmp(command_token, "exit") == 0) { // exit command
    exit_command(0);
    return;
  }
  else if (strcmp(command_token, "echo") == 0) { // echo command
    for (int i = 1; i < count; i++) {
      echo_command(command[i]);
    }
    printf("\n");
    return;
  }
  else if (strcmp(command_token, "pwd") == 0) { // pwd command
    if (*cwd == NULL) {
      fprintf(stderr, "Current working directory not found");
      exit_command(1);
    }
    printf("%s\n", *cwd);
    return;
  }
  else if (strcmp(command_token, "cd") == 0) {
    if (count > 2) {
      fprintf(stderr, "%s: too many arguments", command_token);
      return;
    }
    char *temp = command[1];
    char *to = NULL;
    if (check_path_to_dir(temp, *cwd, &to)) {
      free(*cwd);
      if (to == NULL) // Absolute path
        *cwd = strdup(temp);
      else {
        *cwd = strdup(to);
        free(to);
      }
      if (chdir(*cwd) != 0) { // Changing the directory accordingly
        perror("There is error changing directory: ");
      }
    }
    else {
      fprintf(stderr, "cd: %s: No such file or directory\n", temp);
    }
    return;
  }
  else if (strcmp(command_token, "type") == 0) { // type command
    for (int i = 1; i < count; i++) {
      type_command(command[i]);
    }
    return;
  }
  else if (strcmp(command_token, "history") == 0) {
    if (count == 1)
      history_command(-1, NULL);
    else if (command[1][0] == '-') { // arugment to handle : )
      if (count != 3) {
        fprintf(stderr, "%d: too many arguments\n", count);
      }
      history_command(-1, command);
    }
    else { // must fall into number
      char *end;
      long val = strtol(command[1], &end, 10);
      if (*end != '\0') {
        fprintf(stderr, "%s: %s: invalid option\n", command_token, command[1]);
      }
      else {
        history_command((int)val, NULL);
      }
    }
  }
  return;
}
void handle_command(char **command, int count, char **cwd, int flag) {
  if (flag == 0) { // No redirect
    if (is_built_in(command[0])) {
      // It is built_in command
      execute_built_in(command, count, cwd);
    }
    else { // check if command exists in path and executable
      execute_existing(command, count);
    }
  }
  else {
    if (is_built_in(command[0]) && is_immutable_to_redirect(command[0])) {
      // It is built_in command and don't need to access shell state ?
      execute_built_in(command, count, cwd);
    }
    else { // Always need spawn new child process to cover this execution
      int cnt = 0;
      char **mod_command;
      pid_t pid = fork();
      if (pid == -1) {
        fprintf(stderr, "Error handling command");
        return;
      }

      if (pid == 0) { // in child process
        // extract new command and set file descriptor
        mod_command = extract_redirect_from_input(command, count, &cnt);
        if (is_built_in(command[0])) {
          execute_built_in(mod_command, cnt, cwd);
        }
        else {
          execute_existing(mod_command, cnt);
        }
        for (int i = 0; i < cnt; i++) {
          free(mod_command[i]);
        }
        free(mod_command);
        exit(0);
      }
      wait(NULL); // wait for the child process
    }
  }
}
// Need to modify the saveptr so **
void execute_existing(char **command, int count) {
  char *command_token = command[0];
  char *temp = check_executable_file_in_path(command_token);
  if (!temp) {
    fprintf(stderr, "%s: command not found", command_token);
    printf("\n");
    return;
  }
  else { // handle the arguement
    char **argument_list = malloc(sizeof(char *) * (count + 1));
    for (int i = 0; i < count; i++) {
      argument_list[i] = strdup(command[i]);
    }
    argument_list[count] = NULL;
    pid_t pid = fork();
    if (pid == 0) { // child process
      execv(temp, argument_list);
      // If error then these code will be executed
      fprintf(stderr, "%s: command not found\n", argument_list[0]);
      exit(1);
    }
    wait(NULL); // wait for child process
    for (int i = 0; i < count; i++) {
      free(argument_list[i]);
    }
    free(argument_list);
    return;
  }
}