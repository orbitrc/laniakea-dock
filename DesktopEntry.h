#ifndef DESKTOPENTRY_H
#define DESKTOPENTRY_H

#include <QString>

#include <QMap>

#include <desktopentry/desktopentry.h>

#define LA_DOCK_APPLICATIONS_PATH "/usr/share/applications"
#define LA_DOCK_LOCAL_APPLICATIONS_PATH "~/.local/share/applications"

class DesktopEntry
{
public:
    DesktopEntry(const char *path);
    ~DesktopEntry();

    void init(const char *path);

    QString entryName() const;
    QString iconPath(size_t width, size_t height) const;

private:
    //======================
    // Helper functions
    //======================
    static bool ends_with(const char *str, const char *cmp);

private:
    desktopentry_desktop *_desktop;
    /// <File name, desktopentry_desktop*> key=value pairs.
    QMap<QString, desktopentry_desktop*> _desktops;
};

#endif // DESKTOPENTRY_H
