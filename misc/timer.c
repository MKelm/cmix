#include <unistd.h>
#include <curses.h>
#include <sys/time.h>
#include <pthread.h>

int max_round_time_seconds = 10;
double max_round_utime = 0;
double remaining_round_utime = 0;

double time_diff(struct timeval x , struct timeval y);

void *timer_output();

int main() {
  max_round_utime = max_round_time_seconds * 1000000;
  remaining_round_utime = max_round_utime;

  pthread_t t_out;
  pthread_create(&t_out, NULL, &timer_output, NULL);

  initscr();

  char input_val[128];
  int i;

  for (i = 0; i < 128; i++) {
    input_val[i] = getch();
    if (remaining_round_utime < 0 || input_val[i] == '\n') {
      input_val[i] = '\0';
      break;
    }
  }

  endwin();

  return 0;
}

void *timer_output() {
  int cy, cx;
  struct timeval before, after;
  while (remaining_round_utime > 0) {
    getyx(stdscr, cy, cx);
    move(2, 1);
    printw("   ");
    move(2, 1);
    printw("%.0lf", remaining_round_utime/1000000);
    move(cy, cx);
    refresh();
    gettimeofday(&before, NULL);
    sleep(1);
    gettimeofday(&after, NULL);
    remaining_round_utime = remaining_round_utime - time_diff(before, after);
  }
  pthread_exit(NULL);
}

double time_diff(struct timeval x , struct timeval y) {
  double x_ms , y_ms , diff;

  x_ms = (double)x.tv_sec*1000000 + (double)x.tv_usec;
  y_ms = (double)y.tv_sec*1000000 + (double)y.tv_usec;

  diff = (double)y_ms - (double)x_ms;

  return diff;
}