#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "phrases.h"

st_phrases phrases_data;
char phrases_lang[56];

void phrases_set_lang(char *lang) {
  strncpy(phrases_lang, lang, sizeof(phrases_lang));
}

void phrases_load(void) {
  int linenum = 0, run = 1;
  char chunk[1024], file_location[520], file_location_parts[10][52];

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
    if (fscanf(f, "%[^\n]\n", &chunk[0]) && strlen(chunk) > 0) {
      switch (linenum) {
        case 0:
          strncpy(phrases_data.column_type, chunk, sizeof(phrases_data.column_type));
          break;
        case 1:
          strncpy(phrases_data.column_date, chunk, sizeof(phrases_data.column_date));
          break;
        case 2:
          strncpy(phrases_data.column_time, chunk, sizeof(phrases_data.column_time));
          break;
        case 3:
          strncpy(phrases_data.column_text, chunk, sizeof(phrases_data.column_text));
          break;
        case 4:
          strncpy(phrases_data.column_cycle, chunk, sizeof(phrases_data.column_cycle));
          break;
        case 5:
          strncpy(phrases_data.title_add_event, chunk, sizeof(phrases_data.title_add_event));
          break;
        case 6:
          strncpy(phrases_data.option_is_birthday, chunk, sizeof(phrases_data.option_is_birthday));
          break;
        case 7:
          strncpy(phrases_data.option_date_text, chunk, sizeof(phrases_data.option_date_text));
          break;
        case 8:
          strncpy(phrases_data.option_type_single, chunk, sizeof(phrases_data.option_type_single));
          break;
        case 9:
          strncpy(phrases_data.option_type_repeating, chunk, sizeof(phrases_data.option_type_repeating));
          break;
        case 10:
          strncpy(phrases_data.option_type_birthday, chunk, sizeof(phrases_data.option_type_birthday));
          break;
      }
      strncpy(chunk, "", sizeof(chunk));
      linenum++;
    } else {
      run = 0;
    }
  } while (run == 1);
  fclose(f);
}