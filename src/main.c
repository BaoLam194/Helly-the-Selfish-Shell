#include "helper.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);
  char *cwd = getcwd(NULL, 0);
  int flag = 1;
  while (flag) {
    // Handle cwd
    printf("$ ");
    char input[MAX_COMMAND_LENGTH];
    fgets(input, sizeof(input), stdin);
    // Remove trailing end-line and add null byte
    input[strcspn(input, "\n")] = '\0';
    //
    // Tokenize the input
    char *saveptr1;
    char *command_token;
    char copy_input[MAX_COMMAND_LENGTH]; // maybe we need input again
    memcpy(copy_input, input, sizeof(input));
    int count = 0;
    char **mod_input = parse_input(copy_input, &count);
    // Take out the command only and check
    if (built_in_command(mod_input, saveptr1, count, &cwd, &flag)) {
      // It is built_in command
    }
    else { // check if command exists in path and executable
      existing_command(command_token, &saveptr1);
    }
  }
  free(cwd);
  return 0;
}
