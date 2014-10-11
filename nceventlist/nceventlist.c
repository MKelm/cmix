/* Ncursesw-C-EventList */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "nceventlist.h"
#include "phrases.h"
#include "display.h"

extern struct phrases phrases_data;

struct list_entry entry_to_change;
struct list_entry list[MAX_LIST_ENTRIES];
int list_length = 0;

char user_file[MAX_FILE_NAME_LENGTH];

int main(int argc, char *argv[]) {
  phrases_load();

  get_user_file();
  printf("%s\n", user_file);

  load_list();

  int input_id = -6;
  do {
    if (input_id == -6) {
      // resize / reset display
      display_init();
      display_init_windows();
      display_menu();
      display_help(0);
      display_list();
    }
    input_id = display_input();

    if (input_id == -2) {
      // add single event
    } else if (input_id == -3) {
      // add repeating event
    } else if (input_id == -4) {
      // add birthday
    } else if (input_id == -5) {
      // delete entry
    }
  } while (input_id != 0);

  save_list();

  return display_end();
}

void get_user_file(void) {
  char user_dir[MAX_FILE_NAME_LENGTH];
  snprintf(
    user_dir, MAX_FILE_NAME_LENGTH, "%s/%s", getenv("HOME"), ".nceventlist"
  );
  mkdir(user_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  snprintf(
    user_file, MAX_FILE_NAME_LENGTH, "%s/%s", user_dir, "data.txt"
  );
}

void load_list(void) {
  FILE *fp;
  if ((fp = fopen(user_file, "r")) == NULL) {
    if ((fp = fopen(user_file, "w+")) == NULL) {
      fprintf(stderr, "Error while loading data file.\n");
      exit(EXIT_FAILURE);
    }
  }
  int i = 0;
  while (1) {
    fscanf(fp, "%hd %hd %hd %hd %hd %s %hd %hd %d",
      &list[i].date.day, &list[i].date.month, &list[i].date.year,
      &list[i].time.hour, &list[i].time.minute,
      list[i].text, &list[i].is_birthday, &list[i].repeat_days,
      &list[i].last_notification_time
    );
    list[i].next_event_time = 0;
    list[i].next_event_time = caclulate_next_event_time(list[i]);
    if (feof(fp)) {
      if (strlen(list[i].text) > 0)
        list_length = i+1;
      break;
    }
    i++;
  }
  fclose(fp);
}

int caclulate_next_event_time(struct list_entry current_entry) {
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  int plus_years = 0;
  if (current_entry.is_birthday == 1) {
    if (current_entry.date.month - 1 < tm.tm_mon ||
        (current_entry.date.month - 1 == tm.tm_mon &&
         current_entry.date.day < tm.tm_mday)) {
      plus_years = 1;
    }
  }

  struct tm new_date_time;
  new_date_time.tm_year = tm.tm_year + plus_years;
  new_date_time.tm_mon = current_entry.date.month - 1;
  new_date_time.tm_mday = current_entry.date.day;
  new_date_time.tm_hour = current_entry.time.hour;
  new_date_time.tm_min = current_entry.time.minute;
  new_date_time.tm_sec = 1;
  new_date_time.tm_isdst = -1;
  int date_time = mktime(&new_date_time);

  if (current_entry.repeat_days > 0) {
    //int current_date_time = mktime(&tm);
    // todo logic for repeat days
  }

  return date_time;
}


void save_list(void) {
  FILE *fp;
  if ((fp = fopen(user_file, "w+")) == NULL) {
    fprintf(stderr, "Error while saving data file.\n");
    exit(EXIT_FAILURE);
  }
  int i = 0;
  while (i < list_length) {
    fprintf(fp, "%hd %hd %hd %hd %hd %s %hd %hd %d",
      list[i].date.day, list[i].date.month, list[i].date.year,
      list[i].time.hour, list[i].time.minute,
      list[i].text, list[i].is_birthday, list[i].repeat_days,
      list[i].last_notification_time
    );
    i++;
  }
  fclose(fp);
}