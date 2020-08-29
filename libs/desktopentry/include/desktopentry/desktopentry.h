#ifndef _DESKTOPENTRY_H
#define _DESKTOPENTRY_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * An entry section.
 */
typedef struct desktopentry_entry {
    char *name;
    uint64_t n_locale_names;
    void *locale_names;
    char *exec;
    char *icon_name;
    void *icons;
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


/*========================*/
/* desktopentry functions */
/*========================*/

/**
 * Free the string that allocated by Rust.
 */
void desktopentry_string_free(char *str);


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
 * Get the desktop entry exec path.
 *
 * @param desktop A desktopentry_desktop object.
 * @return Desktop entry exec string or NULL pointer. The returned value should
 *          not freed.
 */
const char* desktopentry_desktop_entry_exec(const desktopentry_desktop *desktop);

/**
 * Get the proper sized icon path.
 */
char* desktopentry_desktop_get_proper_icon(const desktopentry_desktop *desktop,
        size_t size);

/**
 * Free the memory of desktopentry_desktop object.
 * \param desktop Object want to free.
 */
void desktopentry_desktop_free(desktopentry_desktop *desktop);

#ifdef __cplusplus
}
#endif

#endif /* _DESKTOPENTRY_H */
