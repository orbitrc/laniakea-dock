#ifndef _DESKTOPENTRY_H
#define _DESKTOPENTRY_H

#include <stdint.h>

typedef struct desktopentry_entry {
    char *name;
    uint64_t n_locale_names;
    void *locale_names;
    char *exec;
} desktopentry_entry;

typedef struct desktopentry_action {
} desktopentry_action;

typedef struct desktopentry_desktop {
    desktopentry_entry entry;
} desktopentry_desktop;

desktopentry_desktop* desktopentry_desktop_parse(const char *path);

void desktopentry_desktop_free(desktopentry_desktop *desktop);

#endif /* _DESKTOPENTRY_H */
