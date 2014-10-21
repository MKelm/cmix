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
  int next_event_time;
  int last_notification_time;
} st_list_entry;

typedef struct {
  gchar type[256];
  gchar date[256];
  gchar time[256];
  gchar text[256];
  gint cycle;
} st_gtk_list_item;

st_gtk_list_item get_gtk_list_item(int i);

int list_find_next_idx(st_list_entry *current_entry);
int set_gtk_list_item(st_gtk_list_item *gtk_list_item);

void list_get_file(void);
void list_load(void);

int calculate_next_event_time(st_list_entry *current_entry);
void list_sort(void);

void list_save(void);