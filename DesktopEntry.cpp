#include "DesktopEntry.h"

#include <string.h>
#include <filesystem>

DesktopEntry::DesktopEntry()
{
    this->init();
}

DesktopEntry::~DesktopEntry()
{
    // Free desktopentry_desktop structs.
    for (auto& desktop: this->_desktops) {
        desktopentry_desktop_free(desktop);
    }
}

void DesktopEntry::init()
{
    namespace fs = std::filesystem;

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

QString DesktopEntry::path(const QString& filename) const
{
    desktopentry_desktop *desktop = this->_desktops.value(filename, nullptr);
    if (desktop != nullptr) {
        const char *path = desktopentry_desktop_path(desktop);

        return QString(path);
    }

    return QString();
}

QString DesktopEntry::entryName(const QString& filename) const
{
    desktopentry_desktop *desktop = this->_desktops.value(filename, nullptr);
    if (desktop != nullptr) {
        const char *name = desktopentry_desktop_entry_name(desktop);

        return QString(name);
    }

    fprintf(stderr, "DesktopEntry::entryName - desktop is nullptr");
    return QString();
}

QString DesktopEntry::entryExec(const QString& filename) const
{
    desktopentry_desktop *desktop = this->_desktops.value(filename, nullptr);
    if (desktop != nullptr) {
        const char *exec = desktopentry_desktop_entry_exec(desktop);

        return QString(exec);
    }

    fprintf(stderr, "DesktopEntry::entryExec - desktop is nullptr");
    return QString();
}

QString DesktopEntry::iconPath(const QString& filename, size_t width, size_t height) const
{
    size_t size = width;
    (void)height;

    desktopentry_desktop *desktop = this->_desktops.value(filename, nullptr);
    if (desktop != nullptr) {
        char *path = desktopentry_desktop_get_proper_icon(desktop, size);
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
// Find methods
//=========================
QString DesktopEntry::findFilenameByEntryExec(const QString &entryExec) const
{
    namespace fs = std::filesystem;

    auto end = this->_desktops.keyValueEnd();
    for (auto iter = this->_desktops.keyValueBegin(); iter != end; ++iter) {
        QString exec = desktopentry_desktop_entry_exec(this->_desktops.value(iter->first));
        // Strip desktop entry exec arguments.
        exec = exec.split(' ')[0];
        if (exec == entryExec) {
            return iter->first;
        }
        // Strip paths so get only compare exec filename.
        exec = fs::path(exec.toStdString()).filename().c_str();
        auto cmp = fs::path(entryExec.toStdString()).filename().c_str();
        if (exec == cmp) {
            return iter->first;
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
