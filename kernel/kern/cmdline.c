#include <kern/cmdline.h>
#include <mm/malloc.h>
#include <uthash.h>
#include <utils/kprintf.h>

struct cmdline_option * cmdline_options_table = NULL;

enum parser_state {
    search_option_start,
    search_option_end,
    search_value_start,
    search_value_end
};

bool cmdline_parse(const char * cmdline) {
    enum parser_state state = search_option_start;

    size_t i = 0;
    char c = cmdline[0];
    size_t option_start = -1;
    size_t value_start = -1;

    char * name = 0;
    char * value = 0;
    UT_array *values;
    //kprintf("creating values array\n");
    utarray_new(values, &ut_str_icd);

    while(c) {
        //kprintf("state=%d c=%c\n", state, c);
        switch(state) {
        case search_option_start:
            if(c != ' ') {
                state = search_option_end;
                option_start = i;
            }
            break;
        case search_option_end:
            if(c == ' ') {
                state = search_option_start;
                name = strndup(cmdline + option_start, i - option_start);
                cmdline_option_add(name, values);
                kfree(name);
                name = 0;
            } else if(c == '=') {
                state = search_value_start;
                name = strndup(cmdline + option_start, i - option_start);
            }
            break;
        case search_value_start:
            if(c == ' ') {
                state = search_option_start;
            } else {
                state = search_value_end;
                value_start = i;
            }
            break;
        case search_value_end:
            if(c == ' ') {
                state = search_option_start;
                value = strndup(cmdline + value_start, i - value_start);
                //kprintf("found value %s end at i=%ld start=%ld\n", value, i, value_start);
                value_start = -1;
                utarray_push_back(values, &value);
                cmdline_option_add(name, values);
                kfree(name);
                name = 0;
                utarray_clear(values);
            } else if(c == ',') {
                state = search_value_start;
                value = strndup(cmdline + value_start, i - value_start);
                //kprintf("found value %s end at i=%ld start=%ld\n", value, i, value_start);
                value_start = -1;
                utarray_push_back(values, &value);
            }
            break;
        }

        // loop
        i++;
        c = cmdline[i];
    }

    if(name) {
        //kprintf("found option %s\n", name);
        if(value_start != -1) {
            value = strndup(cmdline + value_start, i - value_start);
            //kprintf("found value %s end at i=%ld start=%ld\n", value, i, value_start);
            utarray_push_back(values, &value);
        }
        cmdline_option_add(name, values);
        kfree(name);
    }

    utarray_free(values);
}

bool cmdline_option_add(const char * name, UT_array * values) {
    assert(name != 0);

    struct cmdline_option * option = kmalloc(sizeof(struct cmdline_option));
    option->name = strdup(name);
    option->value_count = utarray_len(values);
    option->values = kmalloc(sizeof(char*) * option->value_count);
    for(size_t i = 0; i < option->value_count; i++) {
        option->values[i] = strdup(*(char**)utarray_eltptr(values, i));
    }

    HASH_ADD_KEYPTR(hh, cmdline_options_table, option->name, strlen(option->name), option);
}

void cmdline_print_info() {
    struct cmdline_option * option;
    kprintf("command line options:\n");
    for(option = cmdline_options_table; option != NULL; option = option->hh.next) {
        kprintf("%s", option->name);
        if(option->value_count) {
            kprintf("=");
            for(size_t i = 0; i < option->value_count; i++) {
                if(i) {
                    kprintf(",");
                }
                kprintf("%s", option->values[i]);
            }
        }
        kprintf("\n");
    }
}
