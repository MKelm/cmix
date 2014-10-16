#include <unistd.h>
#include <curses.h>
#include <sys/time.h>
#include <pthread.h>

int max_round_time_seconds = 180;
double max_round_utime = 0;
double remaining_round_utime = 0;

double time_diff(struct timeval x , struct timeval y);

void *timer_output();

int main() {
  pthread_t t_out;
  pthread_create(&t_out, NULL, &timer_output, NULL);

  struct timeval before, after;

  max_round_utime = max_round_time_seconds * 1000000;
  remaining_round_utime = max_round_utime;

  initscr();
  halfdelay(10 * max_round_time_seconds);

  char input_val[128];
  int i;
  gettimeofday(&before, NULL);
  for (i = 0; i < 128; i++) {
    input_val[i] = getch();
    if (input_val[i] == '\n') {
      input_val[i] = '\0';
      break;
    }
  }
  gettimeofday(&after, NULL);

  nocbreak();
  endwin();

  printf("Total time elapsed : %.0lf us\n" , time_diff(before, after));

  remaining_round_utime = remaining_round_utime - time_diff(before, after);
  printf("Rest round time %.0lf us\n", remaining_round_utime);

  return 0;
}

void *timer_output() {
  int i, cy, cx;
  for(i = 1; i < 10; i++) {
    getyx(stdscr, cy, cx);
    move(2, i);
    printw("*");
    move(cy, cx);
    refresh();
    sleep(1);
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