#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

/*** defines ***/
#define CTRL_KEY(k) ((k) & 0x1f)


// Stores the terminal's original settings so they can be restored
// before the program exits.
struct termios orig_termios;

// Print an error message (based on errno) and terminate the program.
void die(const char *s) {
    perror(s);
    exit(1);
}

// Restore the terminal to its original (cooked) mode.
// This is automatically called when the program exits.
void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode() {
    // Read the terminal's current configuration.
    // We save it so we can restore it later.
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");

    // Ensure disableRawMode() is called automatically on exit.
    atexit(disableRawMode);

    // Make a copy so we modify only the temporary settings.
    struct termios raw = orig_termios;

    // ==========================
    // Input Flags (Keyboard Input)
    // ==========================
    // BRKINT : Don't generate SIGINT on BREAK.
    // ICRNL  : Don't translate Enter (\r) into '\n'.
    // INPCK  : Disable input parity checking.
    // ISTRIP : Don't strip the 8th bit of each input byte.
    // IXON   : Disable software flow control (Ctrl-S/Ctrl-Q).
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

    // ==========================
    // Output Flags
    // ==========================
    // Disable output processing.
    // For example, '\n' will no longer automatically become "\r\n".
    raw.c_oflag &= ~(OPOST);

    // ==========================
    // Control Flags
    // ==========================
    // Use 8 bits per character.
    raw.c_cflag |= (CS8);

    // ==========================
    // Local Flags
    // ==========================
    // ECHO   : Don't automatically print typed characters.
    // ICANON : Read input byte-by-byte instead of line-by-line.
    // IEXTEN : Disable implementation-defined special characters.
    // ISIG   : Disable Ctrl-C (SIGINT) and Ctrl-Z (SIGTSTP).
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    // ==========================
    // read() Behavior
    // ==========================
    // VMIN = 0  -> read() returns immediately if no input is available.
    // VTIME = 1 -> Wait at most 0.1 seconds for input before returning.
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    // Apply the modified settings.
    // TCSAFLUSH waits until all pending output is written and
    // discards any unread input before enabling raw mode.
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

int main() {
    // Put the terminal into raw mode.
    enableRawMode();

    while (1) {
        char c = '\0';

        // Read one byte from standard input.
        // In raw mode, this returns each key press immediately.
        // If no key is pressed within 100 ms (VTIME),
        // read() returns 0.
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
            die("read");

        // Print control characters (Enter, Backspace, Ctrl keys, etc.)
        // as their numeric ASCII values.
        if (iscntrl(c)) {
            printf("%d\r\n", c);
        }
        // Print printable characters along with their ASCII values.
        else {
            printf("%d ('%c')\r\n", c, c);
        }

        // Quit when the user presses 'q'.
        if (c == CTRL_KEY('q')){
            break;
        }
    }

    // disableRawMode() is automatically called by atexit().
    return 0;
}
