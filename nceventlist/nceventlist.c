/* Ncursesw-C-EventList */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "nceventlist.h"

int main(int argc, char *argv[]) {
  struct list_entry list[MAX_LIST_ENTRIES];
  int list_length = 0;

  char user_file[MAX_FILE_NAME_LENGTH];
  get_user_file(user_file);
  printf("%s\n", user_file);

  load_list(user_file, list, &list_length);

  save_list(user_file, list, &list_length);

  return 0;
}

void get_user_file(char *user_file) {
  char user_dir[MAX_FILE_NAME_LENGTH];
  snprintf(
    user_dir, MAX_FILE_NAME_LENGTH, "%s/%s", getenv("HOME"), ".nceventlist"
  );
  mkdir(user_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  snprintf(
    user_file, MAX_FILE_NAME_LENGTH, "%s/%s", user_dir, "data.txt"
  );
}

void load_list(char *user_file, struct list_entry list[], int *list_length) {
  FILE *fp;
  if ((fp = fopen(user_file, "r")) == NULL) {
    if ((fp = fopen(user_file, "w+")) == NULL) {
      fprintf(stderr, "Error while loading data file.\n");
      exit(EXIT_FAILURE);
    }
  }
  int i = 0;
  while(1) {
    fscanf(fp, "%hd %hd %hd %s %hd %hd %d",
      &list[i].date.day, &list[i].date.month, &list[i].date.year,
      list[i].text, &list[i].is_birthday, &list[i].is_permanent,
      &list[i].last_notification_time
    );
    list[i].next_event_time = 0;
    //liste[i].next_event_time = caclulate_next_event_time(liste[i]);
    if (feof(fp)) {
      if (strlen(list[i].text) > 0)
        *list_length = i+1;
      break;
    }
    i++;
  }
  fclose(fp);
}

void save_list(char *user_file, struct list_entry list[], int *list_length) {
  FILE *fp;
  if ((fp = fopen(user_file, "w+")) == NULL) {
    fprintf(stderr, "Error while saving data file.\n");
    exit(EXIT_FAILURE);
  }
  int i = 0;
  while (i < *list_length) {
    fprintf(fp, "%hd %hd %hd %s %hd %hd %d",
      list[i].date.day, list[i].date.month, list[i].date.year,
      list[i].text, list[i].is_birthday, list[i].is_permanent,
      list[i].last_notification_time
    );
    i++;
  }
  fclose(fp);
}