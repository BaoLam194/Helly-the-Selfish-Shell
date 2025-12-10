#include "helper.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);
  char *cwd = getcwd(NULL, 0);
  while (1) {
    // Handle cwd
    printf("$ ");
    char input[MAX_COMMAND_LENGTH];
    fgets(input, sizeof(input), stdin);
    // Remove trailing end-line and add null byte
    input[strcspn(input, "\n")] = '\0';

    // Tokenize the input
    char copy_input[MAX_COMMAND_LENGTH]; // maybe we need input again
    memcpy(copy_input, input, sizeof(input));
    int count = 0;
    char **mod_input = parse_input(copy_input, &count);
    if (!count) {
      printf("No arguments provided \?\?\?\n");
      return 0;
    }
    // Check which type of command for better handle
    if (is_built_in(mod_input[0])) {
      // It is built_in command
      execute_built_in(mod_input, count, &cwd);
    }
    else { // check if command exists in path and executable
      execute_existing(mod_input, count);
    }
    for (int i = 0; i < count; i++) {
      free(mod_input[i]);
    }
    free(mod_input);
  }
  free(cwd);
  return 0;
}
