#define MAX_LIST_ENTRIES 1000

#define MAX_LIST_ENTRY_TEXT_LENGTH 256

#define MAX_FILE_NAME_LENGTH 1024

typedef struct {
  struct st_list_entry_date {
    short day;
    short month;
    short year;
  } date;
  struct st_list_entry_time {
    short hour;
    short minute;
  } time;
  char text[MAX_LIST_ENTRY_TEXT_LENGTH];
  short is_birthday;
  short repeat_cycle; // in days
  int last_notification_time;
} st_list_entry;

void list_get_file(void);
void list_load(void);
void list_save(void);