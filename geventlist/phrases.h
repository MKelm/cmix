#define FILE_DIR_PHRASES "phrases/"
#define FILE_EXT_PHRASES ".txt"

#define PHRASES_CHARS_LENGTH 256

typedef struct {
  char column_type[PHRASES_CHARS_LENGTH];
  char column_date[PHRASES_CHARS_LENGTH];
  char column_time[PHRASES_CHARS_LENGTH];
  char column_text[PHRASES_CHARS_LENGTH];
  char column_cycle[PHRASES_CHARS_LENGTH];
  char title_add_event[PHRASES_CHARS_LENGTH];
  char option_is_birthday[PHRASES_CHARS_LENGTH];
  char option_date_text[PHRASES_CHARS_LENGTH];
  char option_type_single[PHRASES_CHARS_LENGTH];
  char option_type_repeating[PHRASES_CHARS_LENGTH];
  char option_type_birthday[PHRASES_CHARS_LENGTH];
  char message_event[PHRASES_CHARS_LENGTH];
} st_phrases;

void phrases_set_lang(char *lang);

void phrases_load(void);