#ifndef DESKTOPENTRY_H
#define DESKTOPENTRY_H

#include <QString>

#include <desktopentry/desktopentry.h>

class DesktopEntry
{
public:
    DesktopEntry(const char *path);
    ~DesktopEntry();

    void init(const char *path);

    QString entryName() const;
    QString iconPath(size_t width, size_t height) const;

private:
    desktopentry_desktop *_desktop;
};

#endif // DESKTOPENTRY_H
