#ifndef CMDLINE_H
#define CMDLINE_H

#include <stddef.h>
#include <stdbool.h>
#include <uthash.h>
#include <utarray.h>

struct cmdline_option {
    char * name;
    size_t value_count;
    char ** values;
    UT_hash_handle hh;
};

extern struct cmdline_option * cmdline_options_table;

bool cmdline_parse(const char * cmdline);
bool cmdline_option_add(const char * name, UT_array * values);
void cmdline_print_info(void);

#endif
