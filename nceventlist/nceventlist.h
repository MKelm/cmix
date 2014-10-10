#define MAX_LIST_ENTRIES 1000

#define MAX_LIST_ENTRY_TEXT_LENGTH 256

struct list_entry {
  struct list_entry_date {
    short day;
    short month;
    short year;
  } date;
  char text[MAX_LIST_ENTRY_TEXT_LENGTH];
  int is_birthday;
  int is_permanent;
  int next_event_time;
  int last_notification_time;
};