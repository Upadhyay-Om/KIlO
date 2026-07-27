#include <ctype.h>
#include <stdio.h>
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
    // disable ctrl s & ctrm + m
    raw.c_iflag &= ~(IXON|ICRNL);
    // disable echo mode , ICANON sends all data to terminal char by char instead of line by line,ctrl c , ctrl v  
    raw.c_lflag &= ~(ECHO | ICANON| IEXTEN |ISIG);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
  enableRawMode();
  char c;
  while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q'){
        if(iscntrl(c)){
            printf("%d\n",c);
        }else{
            printf("%d ('%c)\n",c,c);
        }
    };
  return 0;
}

