#define MAX_LIST_ENTRIES 1000

#define MAX_LIST_ENTRY_TEXT_LENGTH 256

struct list_entry {
  struct list_entry_date {
    short day;
    short month;
    short year;
  } date;
  struct list_entry_time {
    short hour;
    short minute;
  } time;
  char text[MAX_LIST_ENTRY_TEXT_LENGTH];
  short is_birthday;
  short repeat_days;
  int next_event_time;
  int last_notification_time;
};

#define MAX_FILE_NAME_LENGTH 1024

void get_user_file(void);

void load_list(void);

int caclulate_next_event_time(struct list_entry current_entry);

void save_list(void);

int add_entry(void);