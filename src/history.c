#include "helper.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
void read_my_history(char *file, bool flag) {
  FILE *fp;
  char buffer[MAX_ARGUMENT_LENGTH];
  if ((fp = fopen(file, "r")) == NULL) {
    if (flag) // Not start up
      fprintf(stderr, "%s: file not found", file);
    return;
  }

  while ((fgets(buffer, sizeof(buffer), fp)) != NULL) {
    buffer[strcspn(buffer, "\n")] = '\0';
    add_history(buffer);
  }
  fclose(fp);
  return;
}
void write_my_history(char *file) {
  FILE *fp;
  if ((fp = fopen(file, "w")) == NULL) {
    fprintf(stderr, "%s: error open file", file);
    return;
  }
  HIST_ENTRY **my_his = history_list();
  int start;
  for (start = 0; my_his[start] != NULL; start++) {
    fprintf(fp, "%s\n", my_his[start]->line);
  }
  fclose(fp);
  return;
}

void append_my_history(char *file) {
  FILE *fp;
  if ((fp = fopen(file, "a")) == NULL) {
    fprintf(stderr, "%s: error open file", file);
    return;
  }
  HIST_ENTRY **my_his = history_list();
  int it;
  for (it = current_offset_for_write; my_his[it] != NULL; it++) {
    fprintf(fp, "%s\n", my_his[it]->line);
  }
  current_offset_for_write = history_length;
  fclose(fp);
  return;
}
