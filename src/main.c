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
    int count = 0, flag = 0; // currently the flag is for redirection, may
                             // change if there is better way to handle
    char **mod_input = parse_input(copy_input, &count, &flag);
    if (!count) {
      printf("No arguments provided \?\?\?\n");
      return 0;
    }
    // Check which type of command for better handle
    handle_command(mod_input, count, &cwd, flag);
    for (int i = 0; i < count; i++) {
      free(mod_input[i]);
    }
    free(mod_input);
  }
  free(cwd);
  return 0;
}
