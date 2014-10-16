#include <unistd.h>
#include <curses.h>
#include <sys/time.h>
#include <pthread.h>

int max_round_seconds = 25;
int remaining_round_seconds = 0;

void *timer_output();

int main() {
  remaining_round_seconds = max_round_seconds;

  pthread_t t_out;
  pthread_create(&t_out, NULL, &timer_output, NULL);

  initscr();

  char input_val[128];
  int i;

  for (i = 0; i < 128; i++) {
    halfdelay(remaining_round_seconds*10);
    input_val[i] = getch();
    if (remaining_round_seconds <= 0 || input_val[i] == '\n') {
      input_val[i] = '\0';
      break;
    } else if (input_val[i] == '\0') {
      i--;
    }
  }

  endwin();

  printf("Input string = %s\n", input_val);
  return 0;
}

void *timer_output() {
  int cy, cx;
  while (remaining_round_seconds >= 0) {
    getyx(stdscr, cy, cx);
    move(2, 1);
    printw("   ");
    move(2, 1);
    printw("%d", remaining_round_seconds);
    move(cy, cx);
    refresh();
    sleep(1);
    remaining_round_seconds = remaining_round_seconds - 1;
  }
  pthread_exit(NULL);
}