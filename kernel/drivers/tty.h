#ifndef TTY_H
#define TTY_H

#include <sys/param.h>

#define TTY_MAX_LINE_SIZE 1024
#define TTY_MAX_LINES 1024
#define TTY_NEW_LINE_CHAR '\n'

struct tty_line {
    char * data;
    size_t capacity;
    size_t length;
    size_t read_pos;
    size_t write_pos;
};

struct tty_observer {
    struct thread * thread;
    struct tty_observer * next;
};

struct tty {
    struct tty_line * lines;
    struct tty_observer * observers;
};

enum tty_key_event_type {
    tty_key_down,
    tty_key_up
};

enum tty_mod_key {
    tty_mod_shift = 1,
    tty_mod_alt = 2,
    tty_mod_ctrl = 4
};

struct tty_key_event {
    enum tty_key_event_type event_type;
    char character;
    enum tty_mod_key mod_keys;
};

void tty_init(void);

/*
 * from the point of view of the application interacting with the user:
 */

// sends a string to the display.
int tty_puts(const char * data);

// writes a character to the display.
int tty_putc(char c);

// reads a line from the keyboard, after buffering and editing. can block.
int tty_gets(char * buffer, size_t maxlength);

// reads a character from the buffer. can block.
int tty_getc(void);

/*
 * from the point of view of the devices interacting with the tty system:
 */

// send a keypress to the line buffer
int tty_send_key_event(struct tty_key_event * event);

/*
 * internal api:
 * mostly line control.
 */
struct tty_line * tty_line_alloc(void);
void tty_line_free(struct tty_line * line);
int tty_line_newline(void);
int tty_line_append_char(char c);
int tty_line_reserve_space(struct tty_line * line, size_t size);
int tty_line_compact(struct tty_line * line);
size_t tty_line_write_space_available(struct tty_line * line);
size_t tty_line_read_chars_available(struct tty_line * line);
int tty_wait_chars_available();
int tty_notify_chars_available();
struct tty_observer * tty_observer_new(struct thread * t);
void tty_observer_delete(struct tty_observer * o);
void tty_compact_buffer(void);

#endif
