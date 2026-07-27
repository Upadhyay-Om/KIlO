#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
struct termios orig_termios;
void disableRawMode() {
    // on exit put orignal settings
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
void enableRawMode() {
    // get current terminal settings
    tcgetattr(STDIN_FILENO, &orig_termios);
    // on exit disableRawMode
    atexit(disableRawMode);

  struct termios raw = orig_termios;
    // disable ECHO mode
    raw.c_lflag &= ~(ECHO);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
  enableRawMode();
  char c;
  while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q');
  return 0;
}

