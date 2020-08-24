#include <desktopentry/desktopentry.h>

#include <stdio.h>

int main()
{
    desktopentry_desktop *desktop = desktopentry_desktop_parse(
        "/usr/share/applications/firefox.desktop");

    printf("Entry Name: %s\n", desktop->entry.name);

    desktopentry_desktop_free(desktop);

    return 0;
}
