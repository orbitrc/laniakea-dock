#include "DesktopEntry.h"

DesktopEntry::DesktopEntry(const char *path)
{
    this->init(path);
}

DesktopEntry::~DesktopEntry()
{
    desktopentry_desktop_free(this->_desktop);
}

void DesktopEntry::init(const char *path)
{
    this->_desktop = desktopentry_desktop_parse(path);
}

QString DesktopEntry::entryName() const
{
    const char *name = desktopentry_desktop_entry_name(this->_desktop);

    return QString(name);
}

QString DesktopEntry::iconPath(size_t width, size_t height) const
{
    size_t size = width;
    (void)height;

    if (this->_desktop != nullptr) {
        char *path = desktopentry_desktop_get_proper_icon(this->_desktop, size);
        fprintf(stderr, "DesktopEntry::iconPath - path: %s\n", path);
        if (path != NULL) {
            auto icon_path = QString(path);
            desktopentry_free_string(path);

            return icon_path;
        }
    }

    return QString();
}
