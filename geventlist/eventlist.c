#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "eventlist.h"
#include "phrases.h"

extern st_phrases phrases_data;

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
    strcpy(type_str, phrases_data.option_type_birthday);
  } else if (list[i].repeat_cycle > 0) {
    strcpy(type_str, phrases_data.option_type_repeating);
  } else {
    strcpy(type_str, phrases_data.option_type_single);
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

int list_find_next_idx(st_list_entry *current_entry) {
  int i;
  for (i = 0; i < list_length - 1; i++) {
    if (current_entry->date.year < list[i].date.year || // greater year
        (current_entry->date.year == list[i].date.year && // greater month
         current_entry->date.month < list[i].date.month) ||
        (current_entry->date.year == list[i].date.year && // greater day
         current_entry->date.month == list[i].date.month &&
         current_entry->date.day < list[i].date.day) |
        (current_entry->date.year == list[i].date.year && // greater hour
         current_entry->date.month == list[i].date.month &&
         current_entry->date.day == list[i].date.day &&
         current_entry->time.hour < list[i].time.hour) ||
        (current_entry->date.year == list[i].date.year && // greater minute
         current_entry->date.month == list[i].date.month &&
         current_entry->date.day == list[i].date.day &&
         current_entry->time.hour == list[i].time.hour &&
         current_entry->time.minute < list[i].time.minute)
       ) {
      return i;
    }
  }
  return i;
}

int set_gtk_list_item(st_gtk_list_item *gtk_list_item) {

  if (list_length + 1 < MAX_LIST_ENTRIES) {

    list[list_length].is_birthday =
      (strcmp(gtk_list_item->type, phrases_data.option_type_birthday) == 0)
      ? 1 : 0;
    list[list_length].repeat_cycle = gtk_list_item->cycle;

    strcpy(list[list_length].text, gtk_list_item->text);
    if (strlen(list[list_length].text) == 0)
      return -1; // not enough text data

    char *ptr;
    ptr = strtok(gtk_list_item->date, ".");
    int i = 0;
    while (ptr != NULL) {
      switch (i) {
        case 0:
          list[list_length].date.day = atoi(ptr);
          break;
        case 1:
          list[list_length].date.month = atoi(ptr);
          break;
        case 2:
          list[list_length].date.year = atoi(ptr);
          break;
      }
      ptr = strtok(NULL, ".");
      i++;
    }
    if (i != 3)
      return -1; // not enough date data

    ptr = strtok(gtk_list_item->time, ":");
    i = 0;
    while (ptr != NULL) {
      switch (i) {
        case 0:
          list[list_length].time.hour = atoi(ptr);
          break;
        case 1:
          list[list_length].time.minute = atoi(ptr);
          break;
      }
      ptr = strtok(NULL, ":");
      i++;
    }
    if (i != 2)
      return -1; // not enough time data

    list[list_length].next_event_time = calculate_next_event_time(&list[list_length]);
    if (list[list_length].next_event_time == -1)
      return -1; // no next event time available

    // set next event time to gtk item
    g_snprintf(gtk_list_item->date, 256, "%s%d.%s%d.%d",
      (list[list_length].date.day < 10) ? "0" : "", list[list_length].date.day,
      (list[list_length].date.month < 10) ? "0" : "", list[list_length].date.month,
      list[list_length].date.year);

    g_snprintf(gtk_list_item->time, 256, "%s%d:%s%d",
      (list[list_length].time.hour < 10) ? "0" : "", list[list_length].time.hour,
       (list[list_length].time.minute < 10) ? "0" : "", list[list_length].time.minute);

    list[list_length].last_notification_time = -1;
    list_length++;

    i = list_find_next_idx(&list[list_length-1]);
    list_sort();
    return i;
  }

  return -1; // max items count reached
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
  time_t t = time(NULL); // current calendar time
  struct tm tm = *localtime(&t); // calendar time to location time

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
  time_t entry_t = mktime(&entry_tm); // change location time struct to calendar time

  if (current_entry->is_birthday == 0 && current_entry->repeat_cycle == 0 &&
      entry_t < t) {
    return -1;
  }

  if (current_entry->is_birthday == 0 && current_entry->repeat_cycle > 0) {
    if (tm.tm_isdst == 1) {
      entry_t += 3600;
    }
    while (entry_t < t) {
      entry_t += current_entry->repeat_cycle * 3600 * 24;
    }
  }

  entry_tm = *localtime(&entry_t); // calendar time to location time
  // set current entry time to next
  current_entry->date.year = entry_tm.tm_year + 1900;
  current_entry->date.month = entry_tm.tm_mon + 1;
  current_entry->date.day = entry_tm.tm_mday;
  current_entry->time.hour = entry_tm.tm_hour;
  current_entry->time.minute = entry_tm.tm_min;

  return entry_t;
}

void list_sort(void) {
  /* sort list by year / month and date for output asc */
  int i, has_change = 0;
  st_list_entry tmp_entry;

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
  if (has_change == 1) list_sort();
}

int list_delete_entry(int entry_idx) {
  int i, is_deleted = 0;
  for (i = 0; i < list_length; i++) {

    if (i == entry_idx) {
      is_deleted = 1;
    } else if (is_deleted == 1) {
      list[i-1] = list[i];
    }
  }
  if (is_deleted == 1) {
    list_length--;
    return 1;
  }
  return 0;
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
          phrases_data.option_type_birthday
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
    list_load();
    list_sort();
    send_next_notification();
    fflush(stdout);
    list_save();
    sleep(sleep_seconds);
  }
  exit(EXIT_SUCCESS);
}