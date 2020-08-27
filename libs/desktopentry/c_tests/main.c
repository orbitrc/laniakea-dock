#include <desktopentry/desktopentry.h>

#include <stdio.h>

int main()
{
    desktopentry_desktop *desktop = desktopentry_desktop_parse(
        "/usr/share/applications/firefox.desktop");

    const char *entry_name = desktopentry_desktop_entry_name(desktop);
    printf("Entry Name: %s\n", entry_name);

    desktopentry_desktop_free(desktop);

    return 0;
}
