#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  printf("$ ");
  char input[512];
  fgets(input, sizeof(input), stdin);
  // Remove trailing end-line and add null byte
  input[strcspn(input, "\n")] = '\0';
  printf("%s: command not found\n", input);
  return 0;
}
