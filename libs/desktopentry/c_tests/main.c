#include <desktopentry/desktopentry.h>

#include <stdio.h>

int main()
{
    desktopentry_desktop *desktop = desktopentry_desktop_parse(
        "/usr/share/applications/filezilla.desktop");

    //=============
    // Entry name
    const char *entry_name = desktopentry_desktop_entry_name(desktop);
    printf("Entry Name: %s\n", entry_name);

    //=============
    // Icon path

    // 52x52
    char *proper_icon = desktopentry_desktop_get_proper_icon(desktop, 48);
    if (proper_icon) {
        printf("Icon: %s\n", proper_icon);
    }
    desktopentry_string_free(proper_icon);

    // scalable
    char *scalable_icon = desktopentry_desktop_get_proper_icon(desktop, 600);
    if (proper_icon) {
        printf("Icon: %s\n", scalable_icon);
    }
    desktopentry_string_free(scalable_icon);

    //=============
    // Entry exec
    const char *entry_exec = desktopentry_desktop_entry_exec(desktop);
    printf("Entry Exec: %s\n", entry_exec);

    desktopentry_desktop_free(desktop);

    return 0;
}
