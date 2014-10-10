#define MAX_LIST_ENTRIES 1000

#define MAX_LIST_ENTRY_TEXT_LENGTH 256

struct list_entry {
  struct list_entry_date {
    short day;
    short month;
    short year;
  } date;
  char text[MAX_LIST_ENTRY_TEXT_LENGTH];
  short is_birthday;
  short is_permanent;
  int next_event_time;
  int last_notification_time;
};

#define MAX_FILE_NAME_LENGTH 1024

void get_user_file(char *user_file);

void load_list(char *user_file, struct list_entry list[], int *list_length);

void save_list(char *user_file, struct list_entry list[], int *list_length);