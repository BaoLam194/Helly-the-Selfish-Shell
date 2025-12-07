#include "helper.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  while (1) {
    printf("$ ");
    char input[512];
    fgets(input, sizeof(input), stdin);
    // Remove trailing end-line and add null byte
    input[strcspn(input, "\n")] = '\0';

    // Tokenize the input
    char *saveptr1;
    char *command_token;
    char copy_input[512];
    memcpy(copy_input, input, sizeof(input));
    command_token = strtok_r(copy_input, " \t", &saveptr1);
    if (strcmp(command_token, "exit") == 0) // exit command
      break;
    else if (strcmp(command_token, "echo") == 0) { // echo command
      printf("%s", saveptr1);
    }
    else if (strcmp(command_token, "type") == 0) {
      if (strcmp(saveptr1, "type") == 0 || strcmp(saveptr1, "exit") == 0 ||
          strcmp(saveptr1, "echo") == 0) {
        printf("%s is a shell builtin", saveptr1);
      }
      else {
        char *temp = check_executable_file_in_path(saveptr1);
        if (!temp)
          printf("%s: not found", saveptr1);
        else {
          printf("%s is %s", saveptr1, temp);
          free(temp); // Free the allocated memory
        }
      }
    }
    else
      printf("%s: command not found", input);
    printf("\n");
  }
  return 0;
}
