#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "phrases.h"

struct phrases phrases_data;
char phrases_lang[56];

void phrases_set_lang(char *lang) {
  strncpy(phrases_lang, lang, sizeof(phrases_lang));
}

void phrases_load(void) {
  int linenum = 0, run = 1;
  char line[1024], file_location[520], file_location_parts[10][52];

  readlink("/proc/self/exe", file_location, sizeof(file_location));

  char *ptr;
  ptr = strtok(file_location, "/");
  int i = 0, j = 0;
  while (ptr != NULL) {
    strncpy(file_location_parts[i], ptr, sizeof(file_location_parts[i]));
    ptr = strtok(NULL, "/");
    i++;
  }
  i--;
  strncpy(file_location, "/", sizeof(file_location));
  for (j = 0; j < i; j++) {
    strcat(file_location, file_location_parts[j]);
    strcat(file_location, "/");
  }

  strcat(file_location, FILE_DIR_PHRASES);
  strcat(file_location, phrases_lang);
  strcat(file_location, FILE_EXT_PHRASES);

  FILE *f;
  if ((f = fopen(file_location, "r")) == NULL) {
    fprintf(stderr, "%s\n", "Error while loading phrases data.");
    exit(EXIT_FAILURE);
  }
  do {
    if (fscanf(f, "%[^\n]\n", &line[0]) && strlen(line) > 0) {
      switch (linenum) {
        case 0:
          strncpy(phrases_data.title_menu, line, sizeof(phrases_data.title_menu));
          break;
        case 1:
          strncpy(phrases_data.title_help, line, sizeof(phrases_data.title_help));
          break;
        case 2:
          strncpy(phrases_data.title_list, line, sizeof(phrases_data.title_list));
          break;
        case 3:
          strncpy(phrases_data.info_no_entries, line, sizeof(phrases_data.info_no_entries));
          break;
        case 4:
          strncpy(phrases_data.menu_single_event, line, sizeof(phrases_data.menu_single_event));
          break;
        case 5:
          strncpy(phrases_data.menu_repeating_event, line, sizeof(phrases_data.menu_repeating_event));
          break;
        case 6:
          strncpy(phrases_data.menu_birthday_event, line, sizeof(phrases_data.menu_birthday_event));
          break;
        case 7:
          strncpy(phrases_data.menu_exit, line, sizeof(phrases_data.menu_exit));
          break;
        case 8:
          strncpy(phrases_data.help_menu, line, sizeof(phrases_data.help_menu));
          break;
        case 9:
          strncpy(phrases_data.help_list, line, sizeof(phrases_data.help_list));
          break;
        case 10:
          strncpy(phrases_data.help_single_event, line, sizeof(phrases_data.help_single_event));
          break;
        case 11:
          strncpy(phrases_data.help_repeating_event, line, sizeof(phrases_data.help_repeating_event));
          break;
        case 12:
          strncpy(phrases_data.help_birthday_event, line, sizeof(phrases_data.help_birthday_event));
          break;
        case 13:
          strncpy(phrases_data.entry_birthday, line, sizeof(phrases_data.entry_birthday));
          break;
        case 14:
          strncpy(phrases_data.entry_repeat, line, sizeof(phrases_data.entry_repeat));
          break;
        case 15:
          strncpy(phrases_data.input_single_event, line, sizeof(phrases_data.input_single_event));
          break;
        case 16:
          strncpy(phrases_data.input_repeating_event, line, sizeof(phrases_data.input_repeating_event));
          break;
        case 17:
          strncpy(phrases_data.input_birthday_event, line, sizeof(phrases_data.input_birthday_event));
          break;
        case 18:
          strncpy(phrases_data.input_text, line, sizeof(phrases_data.input_text));
          break;
        case 19:
          strncpy(phrases_data.input_date, line, sizeof(phrases_data.input_date));
          break;
        case 20:
          strncpy(phrases_data.input_time, line, sizeof(phrases_data.input_time));
          break;
        case 21:
          strncpy(phrases_data.input_repeat_cycle, line, sizeof(phrases_data.input_repeat_cycle));
          break;
        case 22:
          strncpy(phrases_data.input_first_name, line, sizeof(phrases_data.input_first_name));
          break;
        case 23:
          strncpy(phrases_data.input_last_name, line, sizeof(phrases_data.input_last_name));
          break;
        case 24:
          strncpy(phrases_data.message_event, line, sizeof(phrases_data.message_event));
          break;
        case 25:
          strncpy(phrases_data.message_error_fread, line, sizeof(phrases_data.message_error_fread));
          break;
        case 26:
          strncpy(phrases_data.message_error_fwrite, line, sizeof(phrases_data.message_error_fwrite));
          break;
      }
      strncpy(line, "", sizeof(line));
      linenum++;
    } else {
      run = 0;
    }
  } while (run == 1);
  fclose(f);
}