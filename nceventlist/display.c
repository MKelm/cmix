/* Geburtstagslistenprogramm Ausgabe */
#include <ncurses.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <ctype.h>
#include "display.h"
#include "phrases.h"
#include "nceventlist.h"

extern struct phrases phrases_data;

extern struct list_entry list[MAX_LIST_ENTRIES];
extern int list_length;

extern struct list_entry entry_to_change;
extern int entry_idx_to_delete;

int maxy, maxx, halfy, halfx;
WINDOW *winput, *whelp, *wlist;
WINDOW *wsinput, *wshelp, *wslist;

/* current_context: 0 = menue, 1 = list */
/* list_offset = display position top */
int current_context, menu_position, list_position, list_offset, list_length_visible;

void display_init() {
  setlocale(LC_ALL, "");
  initscr();
  getmaxyx(stdscr, maxy, maxx);
  halfx = maxx >> 1;
  halfy = maxy >> 1;
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLUE);
  curs_set(0);
  keypad(stdscr, TRUE);

  current_context = 0; // menu
  menu_position = 0;
  list_position = 0;
  list_offset = 0; // offset of list with max. xx entries (visible)
  display_set_list_length();
}

void display_set_list_length(void) {
  list_length_visible = (maxy-2) / 2;
  if (list_length_visible > list_length)
    list_length_visible = list_length;
  if (list_position > list_length_visible-1)
    list_position = list_length_visible-1;
}

void display_menu(void) {
  int c;
  char *menu_entries[4];
  menu_entries[0] = phrases_data.menu_single_event;
  menu_entries[1] = phrases_data.menu_repeating_event;
  menu_entries[2] = phrases_data.menu_birthday_event;
  menu_entries[3] = phrases_data.menu_exit;

  wclear(wsinput);
  for (c = 0; c < 4; c++) {
    if (current_context == 0 && c == menu_position)
      wattron(wsinput, A_REVERSE);
    mvwaddstr(wsinput, (c*2), 0, "* ");
    mvwaddstr(wsinput, (c*2), 2, menu_entries[c]);
    wattroff(wsinput, A_REVERSE);
  }
  wrefresh(wsinput);
}

void display_input_add_event(int is_single) {
  char text[PHRASES_CHARS_LENGTH], day[3], month[3], year[5], hour[3], minute[3], repeat_cycle[5];
  curs_set(1);

  wclear(wsinput);
  if (is_single == 1) {
    mvwaddstr(wsinput, 0, 0, "* ");
    waddstr(wsinput, phrases_data.input_single_event);

  } else {
    mvwaddstr(wsinput, 0, 0, "* ");
    waddstr(wsinput, phrases_data.input_repeating_event);
  }
  mvwaddstr(wsinput, 2, 0, "+ ");
  waddstr(wsinput, phrases_data.input_text);
  waddstr(wsinput, ": ");
  wrefresh(wsinput);
  wgetnstr(wsinput, text, PHRASES_CHARS_LENGTH-1);

  mvwaddstr(wsinput, 4, 0, "+ ");
  waddstr(wsinput, phrases_data.input_date);
  waddstr(wsinput, ":   -  -    ");

  mvwgetnstr(wsinput, 4, 8, day, 2);
  mvwgetnstr(wsinput, 4, 11, month, 2);
  mvwgetnstr(wsinput, 4, 14, year, 4);
  mvwaddstr(wsinput, 5, 0, "+ ");
  waddstr(wsinput, phrases_data.input_time);
  waddstr(wsinput, ":   -  ");
  mvwgetnstr(wsinput, 5, 8, hour, 2);
  mvwgetnstr(wsinput, 5, 11, minute, 2);

  if (is_single == 0) {
    mvwaddstr(wsinput, 7, 0, "+ ");
    waddstr(wsinput, phrases_data.input_repeat_cycle);
    waddstr(wsinput, ": ");
    wgetnstr(wsinput, repeat_cycle, 4);
  }

  if (strlen(text) > 0 &&
      strlen(day) > 0 && strlen(month) > 0 && strlen(year) > 0) {
    strcpy(entry_to_change.text, text);
    entry_to_change.date.day = atoi(day);
    entry_to_change.date.month = atoi(month);
    entry_to_change.date.year = atoi(year);
    entry_to_change.time.hour = atoi(hour);
    entry_to_change.time.minute = atoi(minute);
    entry_to_change.is_birthday = 0;
    entry_to_change.repeat_cycle = atoi(repeat_cycle);
  }

  curs_set(0);
}

void display_input_add_birthday_event(void) {
  char first_name[PHRASES_CHARS_LENGTH/2], last_name[128], day[3], month[3], year[5];
  curs_set(1);

  wclear(wsinput);
  mvwaddstr(wsinput, 0, 0, "* ");
  waddstr(wsinput, phrases_data.input_birthday_event);
  mvwaddstr(wsinput, 2, 0, "+ ");
  waddstr(wsinput, phrases_data.input_first_name);
  waddstr(wsinput, ": ");
  wrefresh(wsinput);
  wgetnstr(wsinput, first_name, 127);
  mvwaddstr(wsinput, 4, 0, "+ ");
  waddstr(wsinput, phrases_data.input_last_name);
  waddstr(wsinput, ": ");
  wrefresh(wsinput);
  wgetnstr(wsinput, last_name, 127);

  mvwaddstr(wsinput, 6, 0, "+ ");
  waddstr(wsinput, phrases_data.input_date);
  waddstr(wsinput, ":   -  -    ");
  mvwgetnstr(wsinput, 6, 8, day, 2);
  mvwgetnstr(wsinput, 6, 11, month, 2);
  mvwgetnstr(wsinput, 6, 14, year, 4);

  if (strlen(first_name) > 0 && strlen(last_name) > 0 &&
      strlen(day) > 0 && strlen(month) > 0 && strlen(year) > 0) {
    strcpy(entry_to_change.text, first_name);
    strcat(entry_to_change.text, " ");
    strcat(entry_to_change.text, last_name);
    entry_to_change.date.day = atoi(day);
    entry_to_change.date.month = atoi(month);
    entry_to_change.date.year = atoi(year);
    entry_to_change.time.hour = 0;
    entry_to_change.time.minute = 0;
    entry_to_change.is_birthday = 1;
    entry_to_change.repeat_cycle = 0;
  }

  curs_set(0);
}

void display_input_del(void) {
  entry_idx_to_delete = list_offset + list_position;
  if (list_length == 1)
    current_context = 0;
}

void display_help(int textid) {
  char *help_texts[5];
  help_texts[0] = phrases_data.help_menu;
  help_texts[1] = phrases_data.help_list;
  help_texts[2] = phrases_data.help_single_event;
  help_texts[3] = phrases_data.help_repeating_event;
  help_texts[4] = phrases_data.help_birthday_event;

  wclear(wshelp);
  char buffer[2048];
  mvwaddstr(wshelp, 0, 0, dsp_word_wrap(buffer, help_texts[textid], halfx-6));
  wrefresh(wshelp);
}

char *dsp_word_wrap(char* buffer, char* string, int line_width) {
  int i = 0;
  int k, counter;

  while (i < strlen(string)) {
    // copy string until the end of the line is reached
    for (counter = 1; counter <= line_width; counter++) {
      // check if end of string reached
      if (i == strlen(string)) {
        buffer[i] = 0;
        return buffer;
      }
      buffer[i] = string[i];
      // check for newlines embedded in the original input
      // and reset the index
      if (buffer[i] == '\n') {
        counter = 1;
      }
      i++;
    }
    // check for whitespace
    if (isspace(string[i])) {
      buffer[i] = '\n';
      i++;
    } else {
      // check for nearest whitespace back in string
      for (k = i; k > 0; k--) {
        if (isspace(string[k])) {
          buffer[k] = '\n';
          // set string index back to character after this one
          i = k + 1;
          break;
        }
      }
    }
  }
  buffer[i] = 0;

  return buffer;
}

void display_list(void) {
  int c;
  char text[256];

  wclear(wslist);
  for (c = list_offset; c < list_offset + list_length_visible; c++) {
    if (current_context == 1 && c - list_offset == list_position)
      wattron(wslist, A_REVERSE);

    snprintf(
      text, sizeof(text),
      "* %d.%d.%d: %s %s",
      list[c].date.day, list[c].date.month, list[c].date.year, list[c].text,
      ((list[c].is_birthday == 1) ?
        phrases_data.entry_birthday : (list[c].repeat_cycle > 0) ?
          phrases_data.entry_repeat : "")
    );
    // todo time output
    mvwaddstr(wslist, 1 + ((c - list_offset) * 2), 1, text);
    wattroff(wslist, A_REVERSE);
  }
  if (c == 0)
    mvwaddstr(wslist, 1 + (0 * 2), 1, phrases_data.info_no_entries);

  wrefresh(wslist);
}

void display_init_windows(void) {
  winput = newwin(halfy, halfx, 0, 0);
  wsinput = subwin(winput, halfy-3, halfx-3, 2, 2);
  whelp = newwin(halfy, halfx, halfy, 0);
  wshelp = subwin(whelp, halfy-3, halfx-3, halfy+2, 2);
  wlist = newwin(maxy, halfx, 0, halfx);
  wslist = subwin(wlist, maxy-2, halfx-2, 1, halfx+1);
  refresh();

  wbkgd(winput, COLOR_PAIR(1));
  wbkgd(wsinput, COLOR_PAIR(1));
  box(winput, 0, 0);
  wattron(winput, A_BOLD);
  mvwaddstr(winput, 0, 2, phrases_data.title_menu);
  wattroff(winput, A_BOLD);
  wrefresh(winput);

  wbkgd(whelp, COLOR_PAIR(1));
  wbkgd(wshelp, COLOR_PAIR(1));
  box(whelp, 0, 0);
  wattron(whelp, A_BOLD);
  mvwaddstr(whelp, 0, 2, phrases_data.title_help);
  wattroff(whelp, A_BOLD);
  wrefresh(whelp);

  wbkgd(wlist, COLOR_PAIR(1));
  wbkgd(wslist, COLOR_PAIR(1));
  box(wlist, 0, 0);
  wattron(wlist, A_BOLD);
  mvwaddstr(wlist, 0, 2, phrases_data.title_list);
  wattroff(wlist, A_BOLD);
  scrollok(wlist, TRUE);
  wrefresh(wlist);
}

int display_input(void) {
  int ch = getch();
  switch (ch) {
    case KEY_RESIZE:
      return -6;
    case KEY_DOWN:
      if (current_context == 0 && menu_position < 3) {
        menu_position++;
        display_menu();
      } else if (current_context == 1) {
        if (list_position < list_length_visible-1) {
          list_position++;
          display_list();
        } else if (list_length > list_length_visible &&
                   list_offset + list_length_visible < list_length) {
          list_offset++;
          display_list();
        }
      }
      return -1;
      break;
    case KEY_UP:
      if (current_context == 0 && menu_position > 0) {
        menu_position--;
        display_menu();
      } else if (current_context == 1) {
        if (list_position > 0) {
          list_position--;
          display_list();
        } else if (list_offset > 0) {
          list_offset--;
          display_list();
        }
      }
      return -1;
      break;
    case KEY_LEFT:
      if (current_context == 1) {
        current_context = 0;
        display_help(0);
        display_menu();
        display_list();
      }
      return -1;
      break;
    case KEY_RIGHT:
      if (current_context == 0 && list_length > 0) {
        current_context = 1;
        display_help(1);
        display_menu();
        display_list();
      }
      return -1;
      break;
    case KEY_DL:
    case KEY_DC:
      if (current_context == 1) {
        // delete entry
        display_input_del();
        if (current_context == 0) {
          display_help(0);
          display_menu();
        }
        //display_list();
        return -5;
      }
    case '\n':
      if (current_context == 0) {
        switch (menu_position) {
          case 0:
            // add single event entry
            display_help(2);
            display_input_add_event(1);
            display_help(0);
            display_menu();
            //display_list();
            return -2;
            break;
          case 1:
            // add repeating event entry
            display_help(3);
            display_input_add_event(0);
            display_help(0);
            display_menu();
            //display_list();
            return -3;
            break;
          case 2:
            // add birthday event entry
            display_help(4);
            display_input_add_birthday_event();
            display_help(0);
            display_menu();
            //display_list();
            return -4;
            break;
          case 3:
            // beenden
            return 0;
            break;
        }
      }
    default:
      return -1;
  }
  return 0;
}

int display_end(void) {
  curs_set(1);
  endwin();
  return 0;
}