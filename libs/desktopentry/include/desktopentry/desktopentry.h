#ifndef _DESKTOPENTRY_H
#define _DESKTOPENTRY_H

#include <stdint.h>

/**
 * An entry section.
 */
typedef struct desktopentry_entry {
    char *name;
    uint64_t n_locale_names;
    void *locale_names;
    char *exec;
} desktopentry_entry;

/**
 * An action section.
 */
typedef struct desktopentry_action {
} desktopentry_action;

/**
 * An .desktop file structure.
 */
typedef struct desktopentry_desktop {
    desktopentry_entry entry;
} desktopentry_desktop;


/*========================================*/
/* desktopentry_desktop functions/methods */
/*========================================*/

/**
 * Parse the .desktop file given path and returns desktopentry_desktop object.
 * \param path File path of .desktop file.
 */
desktopentry_desktop* desktopentry_desktop_parse(const char *path);

/**
 * Get the desktop entry name.
 */
const char* desktopentry_desktop_entry_name(const desktopentry_desktop *desktop);

/**
 * Free the memory of desktopentry_desktop object.
 * \param desktop Object want to free.
 */
void desktopentry_desktop_free(desktopentry_desktop *desktop);

#endif /* _DESKTOPENTRY_H */
