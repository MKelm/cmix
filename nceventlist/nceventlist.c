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

  char lang[128] = "en-US";
  int i, notify_time_cycle = 0;

  if (argc > 2) {
    for (i = 1; i < argc; i++) {
      if (strcmp(argv[i], "-notify") == 0 && atoi(argv[i+1]) > 0) {
        notify_time_cycle = atoi(argv[i+1]);
      }
      if (strcmp(argv[i], "-lang") == 0 && strlen(argv[i+1]) > 0) {
        strncpy(lang, argv[i+1], sizeof(lang));
      }
      i++;
    }
  }

  phrases_set_lang(lang);
  phrases_load();
  get_user_file();

  if (notify_time_cycle > 0) {
    notification_service(notify_time_cycle);
  }

  load_list();
  sort_list();

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
      fprintf(stderr, "%s\n", phrases_data.message_error_fread);
      exit(EXIT_FAILURE);
    }
  }
  int i = 0;
  while (!feof(fp)) {
    fscanf(fp, "%hd %hd %hd %hd %hd {%[^}^\n]} %hd %hd %d\n",
      &list[i].date.day, &list[i].date.month, &list[i].date.year,
      &list[i].time.hour, &list[i].time.minute,
      list[i].text, &list[i].is_birthday, &list[i].repeat_cycle,
      &list[i].last_notification_time
    );
    if (strlen(list[i].text) > 0) {
      list[i].next_event_time = calculate_next_event_time(&list[i]);
      list_length++;
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
         current_entry->date.day < tm.tm_mday) ||
        (current_entry->date.month - 1 == tm.tm_mon &&
         current_entry->date.day == tm.tm_mday &&
         current_entry->time.hour < tm.tm_hour) ||
        (current_entry->date.month - 1 == tm.tm_mon &&
         current_entry->date.day == tm.tm_mday &&
         current_entry->time.hour == tm.tm_hour &&
         current_entry->time.minute < tm.tm_min)) {
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
  entry_tm.tm_isdst = tm.tm_isdst;
  time_t entry_t = mktime(&entry_tm);

  if (current_entry->is_birthday == 0 && current_entry->repeat_cycle > 0) {
    while (entry_t < t) {
      entry_t += current_entry->repeat_cycle * 3600 * 24;
    }
    entry_tm = *localtime(&entry_t);
    // handle time diff, isdst 1 = summer time, 0 = winter time
    if (tm.tm_isdst == 1 && entry_tm.tm_isdst == 0) {
      entry_t += 3600;
    } else if (tm.tm_isdst == 0 && entry_tm.tm_isdst == 1) {
      entry_t -= 3600;
    }
  }

  entry_tm = *localtime(&entry_t);
  // set current entry time to next
  current_entry->date.year = entry_tm.tm_year + 1900;
  current_entry->date.month = entry_tm.tm_mon + 1;
  current_entry->date.day = entry_tm.tm_mday;
  current_entry->time.hour = entry_tm.tm_hour;
  current_entry->time.minute = entry_tm.tm_min;

  return entry_t;
}

void save_list(void) {
  FILE *fp;
  if ((fp = fopen(user_file, "w+")) == NULL) {
    fprintf(stderr, "%s.\n", phrases_data.message_error_fwrite);
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

  if (strlen(entry_to_change.text) > 0) {
    if (list_length + 1 < MAX_LIST_ENTRIES) {
      list[list_length] = entry_to_change;
      list[list_length].next_event_time = calculate_next_event_time(&list[list_length]);
      list[list_length].last_notification_time = -1;
      list_length++;

      strcpy(entry_to_change.text, "");
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
        list[i-1] = list[i];
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
    if (list[i].date.year > list[i+1].date.year || // greater year
        (list[i].date.year == list[i+1].date.year && // greater month
         list[i].date.month > list[i+1].date.month) ||
        (list[i].date.year == list[i+1].date.year && // greater day
         list[i].date.month == list[i+1].date.month &&
         list[i].date.day > list[i+1].date.day) ||
        (list[i].date.year == list[i+1].date.year && // greater hour
         list[i].date.month == list[i+1].date.month &&
         list[i].date.day == list[i+1].date.day &&
         list[i].time.hour > list[i+1].time.hour) ||
        (list[i].date.year == list[i+1].date.year && // greater minute
         list[i].date.month == list[i+1].date.month &&
         list[i].date.day == list[i+1].date.day &&
         list[i].time.hour == list[i+1].time.hour &&
         list[i].time.minute > list[i+1].time.minute)
       ) {
      has_change = 1;

      tmp_entry = list[i];
      list[i] = list[i+1];
      list[i+1] = tmp_entry;
    }
  }
  if (has_change == 1) sort_list();
}

void send_next_notification(void) {
  char message[256], call[256];
  int i;
  time_t t = time(NULL);
  int w2pre, w1pre, d3pre, d1pre;

  for (i = 0; i < list_length; i++) {
    w2pre = list[i].next_event_time - (2 * 7 * 24 * 60 * 60);
    w1pre = list[i].next_event_time - (1 * 7 * 24 * 60 * 60);
    d3pre = list[i].next_event_time - (3 * 24 * 60 * 60);
    d1pre = list[i].next_event_time - (1 * 24 * 60 * 60);

    if ((t > w2pre && t < w1pre && list[i].last_notification_time < w2pre) ||
        (t > w1pre && t < d3pre && list[i].last_notification_time < w1pre) ||
        (t > d3pre && t < d1pre && list[i].last_notification_time < d3pre) ||
        (t > d1pre && t < list[i].next_event_time && list[i].last_notification_time < d1pre)) {

      list[i].last_notification_time = t;

      if (list[i].is_birthday == 1) {
        snprintf(
          message, sizeof(message), "%s%hd.%s%hd. %s %s",
          (list[i].date.day < 10) ? "0" : "", list[i].date.day,
          (list[i].date.month < 10) ? "0" : "", list[i].date.month, list[i].text,
          phrases_data.entry_birthday
        );
      } else {
        snprintf(
          message, sizeof(message), "%s%hd.%s%hd. %s%hd:%s%hd %s",
          (list[i].date.day < 10) ? "0" : "", list[i].date.day,
          (list[i].date.month < 10) ? "0" : "", list[i].date.month,
          (list[i].time.hour < 10) ? "0" : "", list[i].time.hour,
          (list[i].time.minute < 10) ? "0" : "", list[i].time.minute, list[i].text
        );
      }

      snprintf(
        call,
        sizeof(call),
        "%s \"%s\" \"%s\"",
        "notify-send -i /usr/share/icons/gnome/48x48/status/appointment-soon.png",
        phrases_data.message_event,
        message
      );
      system(call);
    }
  }
}

void notification_service(int sleep_seconds) {
  while (1) {
    load_list();
    sort_list();
    send_next_notification();
    fflush(stdout);
    save_list();
    sleep(sleep_seconds);
  }
  exit(EXIT_SUCCESS);
}