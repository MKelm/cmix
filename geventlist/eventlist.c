#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "eventlist.h"

st_list_entry list[MAX_LIST_ENTRIES];
int list_length = 0;

char user_file[MAX_FILE_NAME_LENGTH];

void list_get_file(void) {
  char user_dir[MAX_FILE_NAME_LENGTH];
  snprintf(
    user_dir, MAX_FILE_NAME_LENGTH, "%s/%s", getenv("HOME"), ".nceventlist"
  );
  mkdir(user_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  snprintf(
    user_file, MAX_FILE_NAME_LENGTH, "%s/%s", user_dir, "data.txt"
  );
}

st_gtk_list_item get_gtk_list_item(int i) {
  static st_gtk_list_item gtk_list_item;

  gchar type_str[256];
  if (list[i].is_birthday == 1) {
    strcpy(type_str, "Birthday");
  } else if (list[i].repeat_cycle > 0) {
    strcpy(type_str, "Repeating");
  } else {
    strcpy(type_str, "Single");
  }
  g_snprintf(gtk_list_item.type, 256, "%s", type_str);

  g_snprintf(gtk_list_item.date, 256, "%s%d.%s%d.%d",
    (list[i].date.day < 10) ? "0" : "", list[i].date.day,
    (list[i].date.month < 10) ? "0" : "", list[i].date.month,
    list[i].date.year);

  g_snprintf(gtk_list_item.time, 256, "%s%d:%s%d",
    (list[i].time.hour < 10) ? "0" : "", list[i].time.hour,
     (list[i].time.minute < 10) ? "0" : "", list[i].time.minute);

  g_snprintf(gtk_list_item.text, 256, "%s", list[i].text);

  gtk_list_item.cycle = list[i].repeat_cycle;

  return gtk_list_item;
}

void list_load(void) {
  FILE *fp;
  if ((fp = fopen(user_file, "r")) == NULL) {
    if ((fp = fopen(user_file, "w+")) == NULL) {
      fprintf(stderr, "%s\n", "Error read user file.");
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

int calculate_next_event_time(st_list_entry *current_entry) {
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

void list_save(void) {
  FILE *fp;
  if ((fp = fopen(user_file, "w+")) == NULL) {
    fprintf(stderr, "%s.\n", "Error write user file.");
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