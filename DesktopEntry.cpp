#include "DesktopEntry.h"

#include <string.h>
#include <filesystem>

DesktopEntry::DesktopEntry(const char *path)
{
    this->init(path);
}

DesktopEntry::~DesktopEntry()
{
    desktopentry_desktop_free(this->_desktop);

    // Free desktopentry_desktop structs.
    for (auto& desktop: this->_desktops) {
        desktopentry_desktop_free(desktop);
    }
}

void DesktopEntry::init(const char *path)
{
    namespace fs = std::filesystem;

    this->_desktop = desktopentry_desktop_parse(path);

    // Collect .desktop files in applications directory.
    auto dir_iter = fs::directory_iterator(LA_DOCK_APPLICATIONS_PATH);
    for (auto& file: dir_iter) {
        auto path = file.path().c_str();
        if (this->ends_with(path, ".desktop")) {
            desktopentry_desktop *desktop = desktopentry_desktop_parse(path);
            this->_desktops.insert(file.path().filename().c_str(), desktop);
        }
    }
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
            desktopentry_string_free(path);

            return icon_path;
        }
    }

    return QString();
}

//=========================
// Helper functions
//=========================
bool DesktopEntry::ends_with(const char *str, const char *cmp)
{
    size_t cmp_len = strlen(cmp);

    if (strlen(str) < cmp_len) {
        return false;
    }
    const char *end = str + (strlen(str) - 1);
    end -= (cmp_len - 1);

    return (strcmp(end, cmp) == 0);
}
