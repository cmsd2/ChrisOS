#include <drivers/tty.h>
#include <mm/malloc.h>
#include <utlist.h>
#include <utils/mem.h>
#include <sys/scheduler.h>
#include <terminal/terminal.h>
#include <utils/kprintf.h>

// currently only a single console
struct tty _tty;

void tty_init() {
    memset(&_tty, 0, sizeof(struct tty));
    _tty.lines = tty_line_alloc();
}

struct tty_line * tty_line_alloc() {
    struct tty_line * result = (struct tty_line*)kmalloc(sizeof(struct tty_line));
    memset(result, 0, sizeof(struct tty_line));
    return result;
}

void tty_line_free(struct tty_line * line) {
    if(line->data) {
        kfree(line->data);
        line->data = 0;
    }
    kfree(line);
}

int tty_puts(const char * data, size_t length) {
    for(size_t i = 0; i < length; i++) {
        if(tty_putc(data[i]) != 0) {
            return -1;
        }
    }

    return 0;
}

int tty_gets(char * buffer, size_t maxlength) {
    struct tty_line * line = _tty.lines;

    char c = 0;
    size_t i = 0;
    while(c != TTY_NEW_LINE_CHAR && i < maxlength) {
        c = tty_getc();
        if(c < 0) {
            return c;
        }
        *buffer = c;
        buffer++;
        i++;
    }

    return i;
}

int tty_getc() {
    tty_wait_chars_available();

    struct tty_line * line = _tty.lines;
    if(tty_line_read_chars_available(line) == 0) {
        return -1;
    }

    char c = line->data[line->read_pos];
    line->read_pos++;

    return c;
}

int tty_putc(char c) {
    terminal_putchar(c);
    return 0;
}

int tty_send_key_event(struct tty_key_event * event) {
    if(event->event_type == tty_key_down) {
        if(event->mod_keys == 0 || event->mod_keys == tty_mod_shift) {
            if(event->character == TTY_NEW_LINE_CHAR) {
                tty_line_newline();
            } else {
                tty_line_append_char(event->character);
            }
        }
    }
}

void tty_compact_buffer() {
    struct tty_line * line = _tty.lines;
    memmove(line->data, line->data + line->read_pos, line->length);
    line->length -= line->read_pos;
    line->write_pos -= line->read_pos;
    line->read_pos = 0;
}

int tty_line_newline() {
    if(0 == tty_line_append_char('\n')) {
        tty_compact_buffer();
        tty_notify_chars_available();
        return 0;
    } else {
        return -1;
    }
}

int tty_line_append_char(char c) {
    struct tty_line * line = _tty.lines;

    if(c == 0x08) {
        if(line->read_pos < line->write_pos) {
            line->write_pos--;
            line->length--;

            terminal_putchar(c);
        }
    } else {
        if(tty_line_reserve_space(line, 1)) {
            return -1;
        }

        assert(tty_line_write_space_available(line) > 0);

        line->data[line->write_pos] = c;
        line->write_pos++;
        line->length++;

        terminal_putchar(c);
    }

    return 0;
}

int tty_line_reserve_space(struct tty_line * line, size_t size) {
    assert(line);
    size_t space = tty_line_write_space_available(line);
    if(space < size) {
        size_t new_capacity = MAX(size, line->capacity * 2);
        char * new_data = (char*)krealloc(line->data, new_capacity);
        if(new_data) {
            line->data = new_data;
            line->capacity = new_capacity;
        } else {
            return -1;
        }
    }
    return 0;
}

size_t tty_line_write_space_available(struct tty_line * line) {
    assert(line);
    return line->capacity - line->write_pos;
}

size_t tty_line_read_chars_available(struct tty_line * line) {
    assert(line);
    return line->length - line->read_pos;
}

struct tty_observer * tty_observer_new(struct thread * t) {
    struct tty_observer * observer = (struct tty_observer *)kmalloc(sizeof(struct tty_observer));
    memset(observer, 0, sizeof(struct tty_observer));
    observer->thread = t;

    return observer;
}

void tty_observer_delete(struct tty_observer * o) {
    kfree(o);
}

int tty_wait_chars_available() {
    size_t available;
    struct tty_observer * observer;

    available = tty_line_read_chars_available(_tty.lines);
    if(!available) {
        observer = tty_observer_new(current_thread());
        LL_PREPEND(_tty.observers, observer);

        do {
            scheduler_make_blocked(current_thread());
            scheduler_yield();
            available = tty_line_read_chars_available(_tty.lines);
        } while(!available);
        LL_DELETE(_tty.observers, observer);
        tty_observer_delete(observer);
    }

    if(available != 0) {
        return 0;
    } else {
        return -1;
    }
}

int tty_notify_chars_available() {
    struct tty_observer * elt;
    LL_FOREACH(_tty.observers, elt) {
        scheduler_make_runnable(elt->thread);
    }
}
