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

struct list_entry list[MAX_LIST_ENTRIES];
int list_length = 0;

struct list_entry entry_to_change;
int entry_idx_to_delete = -1;

char user_file[MAX_FILE_NAME_LENGTH];

int main(int argc, char *argv[]) {
  phrases_load();

  get_user_file();
  printf("%s\n", user_file);

  load_list();

  int input_id = -6, data_changed = 0;
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

    switch (input_id) {
      case -2: // add single event
      case -3: // add repeating event
      case -4: // add birthday event
        data_changed = add_entry();
        break;
      case -5:
        data_changed = del_entry();
        break;
    }

    if (data_changed == 1) {
      sort_list();
      display_set_list_length();
      display_list();
      data_changed = 0;
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
    fscanf(fp, "%hd %hd %hd %hd %hd {%[^}]} %hd %hd %d\n",
      &list[i].date.day, &list[i].date.month, &list[i].date.year,
      &list[i].time.hour, &list[i].time.minute,
      list[i].text, &list[i].is_birthday, &list[i].repeat_cycle,
      &list[i].last_notification_time
    );
    list[i].next_event_time = calculate_next_event_time(&list[i]);
    if (feof(fp)) {
      if (strlen(list[i].text) > 0)
        list_length = i+1;
      break;
    }
    i++;
  }
  fclose(fp);
}

int calculate_next_event_time(struct list_entry *current_entry) {
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  int plus_years = 0;
  if (current_entry->is_birthday == 1) {
    if (current_entry->date.month - 1 < tm.tm_mon ||
        (current_entry->date.month - 1 == tm.tm_mon &&
         current_entry->date.day < tm.tm_mday)) {
      plus_years = 1;
    }
  }

  struct tm entry_tm;
  entry_tm.tm_year = tm.tm_year + plus_years;
  entry_tm.tm_mon = current_entry->date.month - 1;
  entry_tm.tm_mday = current_entry->date.day;
  entry_tm.tm_hour = current_entry->time.hour;
  entry_tm.tm_min = current_entry->time.minute;
  entry_tm.tm_sec = 0;
  entry_tm.tm_isdst = -1;
  time_t entry_t = mktime(&entry_tm);

  if (current_entry->is_birthday == 0) {
    while (entry_t < t) {
      entry_t += current_entry->repeat_cycle * 60 * 60 * 24;
    }
    entry_tm = *localtime(&entry_t);
    // set current entry time to next
    current_entry->date.year = entry_tm.tm_year + 1900;
    current_entry->date.month = entry_tm.tm_mon + 1;
    current_entry->date.day = entry_tm.tm_mday;
    current_entry->time.hour = entry_tm.tm_hour;
    current_entry->time.minute = entry_tm.tm_min;
  }
  return entry_t;
}

void save_list(void) {
  FILE *fp;
  if ((fp = fopen(user_file, "w+")) == NULL) {
    fprintf(stderr, "Error while saving data file.\n");
    exit(EXIT_FAILURE);
  }
  int i = 0;
  while (i < list_length) {
    fprintf(fp, "%hd %hd %hd %hd %hd {%s} %hd %hd %d\n",
      list[i].date.day, list[i].date.month, list[i].date.year,
      list[i].time.hour, list[i].time.minute,
      list[i].text, list[i].is_birthday, list[i].repeat_cycle,
      list[i].last_notification_time
    );
    i++;
  }
  fclose(fp);
}

int add_entry(void) {

  if (strlen(entry_to_change.text) > 0 &&
      entry_to_change.date.day > 0 &&
      entry_to_change.date.month > 0 &&
      entry_to_change.date.year > 0) {

    if (list_length + 1 < MAX_LIST_ENTRIES) {
      strcpy(list[list_length].text, entry_to_change.text);
      list[list_length].date.day = entry_to_change.date.day;
      list[list_length].date.month = entry_to_change.date.month;
      list[list_length].date.year = entry_to_change.date.year;
      list[list_length].time.hour = entry_to_change.time.hour;
      list[list_length].time.minute = entry_to_change.time.minute;
      list[list_length].is_birthday = entry_to_change.is_birthday;
      list[list_length].repeat_cycle = entry_to_change.repeat_cycle;
      list[list_length].next_event_time = calculate_next_event_time(&list[list_length]);
      list[list_length].last_notification_time = -1;
      list_length++;

      strcpy(entry_to_change.text, "");
      entry_to_change.date.day = 0;
      entry_to_change.date.month = 0;
      entry_to_change.date.year = 0;
      entry_to_change.time.hour = 0;
      entry_to_change.time.minute = 0;
      entry_to_change.is_birthday = 0;
      entry_to_change.repeat_cycle = 0;

      return 1;
    }
  }
  return 0;
}

int del_entry(void) {
  if (entry_idx_to_delete > -1) {
    int i, is_deleted = 0;
    for (i = 0; i < list_length; i++) {

      if (i == entry_idx_to_delete) {
        is_deleted = 1;
      } else if (is_deleted == 1) {
        strcpy(list[i-1].text, list[i].text);
        list[i-1].date.day = list[i].date.day;
        list[i-1].date.month = list[i].date.month;
        list[i-1].date.year = list[i].date.year;
        list[i-1].time.hour = list[i].time.hour;
        list[i-1].time.minute = list[i].time.minute;
        list[i-1].is_birthday = list[i].is_birthday;
        list[i-1].repeat_cycle = list[i].repeat_cycle;
        list[i-1].next_event_time = list[i].next_event_time;
        list[i-1].last_notification_time = list[i].last_notification_time;
      }
    }
    if (is_deleted == 1) {
      list_length--;
      entry_idx_to_delete = -1;
      return 1;
    }
  }
  return 0;
}

void sort_list(void) {
  /* sort list by year / month and date for output asc */
  int i, has_change = 0;
  struct list_entry tmp_entry;

  for (i = 0; i < list_length - 1; i++) {
    if (list[i].date.year > list[i+1].date.year ||
        (list[i].date.year == list[i+1].date.year &&
         list[i].date.month > list[i+1].date.month) ||
        (list[i].date.year == list[i+1].date.year &&
         list[i].date.month == list[i+1].date.month &&
         list[i].date.day > list[i+1].date.day)
       ) {
      has_change = 1;

      strcpy(tmp_entry.text, list[i].text);
      tmp_entry.date.day = list[i].date.day;
      tmp_entry.date.month = list[i].date.month;
      tmp_entry.date.year = list[i].date.year;
      tmp_entry.time.hour = list[i].time.hour;
      tmp_entry.time.minute = list[i].time.minute;
      tmp_entry.is_birthday = list[i].is_birthday;
      tmp_entry.repeat_cycle = list[i].repeat_cycle;
      tmp_entry.next_event_time = list[i].next_event_time;
      tmp_entry.last_notification_time = list[i].last_notification_time;


      strcpy(list[i].text, list[i+1].text);
      list[i].date.day = list[i+1].date.day;
      list[i].date.month = list[i+1].date.month;
      list[i].date.year = list[i+1].date.year;
      list[i].time.hour = list[i+1].time.hour;
      list[i].time.minute = list[i+1].time.minute;
      list[i].is_birthday = list[i+1].is_birthday;
      list[i].repeat_cycle = list[i+1].repeat_cycle;
      list[i].next_event_time = list[i+1].next_event_time;
      list[i].last_notification_time = list[i+1].last_notification_time;

      strcpy(list[i+1].text, tmp_entry.text);
      list[i+1].date.day = tmp_entry.date.day;
      list[i+1].date.month = tmp_entry.date.month;
      list[i+1].date.year = tmp_entry.date.year;
      list[i+1].time.hour = tmp_entry.time.hour;
      list[i+1].time.minute = tmp_entry.time.minute;
      list[i+1].is_birthday = tmp_entry.is_birthday;
      list[i+1].repeat_cycle = tmp_entry.repeat_cycle;
      list[i+1].next_event_time = tmp_entry.next_event_time;
      list[i+1].last_notification_time = tmp_entry.last_notification_time;
    }
  }
  if (has_change == 1) sort_list();
}