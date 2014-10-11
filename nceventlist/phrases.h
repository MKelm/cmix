#define FILE_PHRASES "phrases/en-US.txt"

#define PHRASES_CHARS_LENGTH 256

struct phrases {
  char title_menu[PHRASES_CHARS_LENGTH];
  char title_help[PHRASES_CHARS_LENGTH];
  char title_list[PHRASES_CHARS_LENGTH];
  char info_no_entries[PHRASES_CHARS_LENGTH];
  char menu_single_event[PHRASES_CHARS_LENGTH];
  char menu_repeating_event[PHRASES_CHARS_LENGTH];
  char menu_birthday_event[PHRASES_CHARS_LENGTH];
  char menu_exit[PHRASES_CHARS_LENGTH];
  char help_menu[PHRASES_CHARS_LENGTH];
  char help_list[PHRASES_CHARS_LENGTH];
  char help_single_event[PHRASES_CHARS_LENGTH];
  char help_repeating_event[PHRASES_CHARS_LENGTH];
  char help_birthday_event[PHRASES_CHARS_LENGTH];
};

void phrases_load(void);