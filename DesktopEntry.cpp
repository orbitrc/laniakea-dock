#include "DesktopEntry.h"

#include <stdlib.h>
#include <string.h>
#include <filesystem>

#include <QRegularExpression>

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
    // Add or override .desktop files in local applications directory.
    QString local_path = QString(LA_DOCK_LOCAL_APPLICATIONS_PATH).replace("~", getenv("HOME"));
    auto local_dir_iter = fs::directory_iterator(local_path.toStdString().c_str());
    for (auto& file: local_dir_iter) {
        auto path = file.path().c_str();
        if (this->ends_with(path, ".desktop")) {
            fprintf(stderr, "Desktop: %s\n", path);
            desktopentry_desktop *desktop = desktopentry_desktop_parse(path);
            // Remove if file in /usr/share/applications.
            if (this->_desktops.contains(file.path().filename().c_str())) {
                auto desktop_p = this->_desktops.value(file.path().filename().c_str());
                desktopentry_desktop_free(desktop_p);
                this->_desktops.remove(file.path().filename().c_str());
            }
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

    fprintf(stderr, "DesktopEntry::entryName - desktop is nullptr\n");
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

QString DesktopEntry::entryIcon(const QString &filename) const
{
    desktopentry_desktop *desktop = this->_desktops.value(filename, nullptr);
    if (desktop != nullptr) {
        const char *icon = desktopentry_desktop_entry_icon(desktop);

        return QString(icon);
    }

    fprintf(stderr, "DesktopEntry::entryIcon - desktop is nullptr\n");
    return QString();
}

QString DesktopEntry::iconPath(const QString& filename, size_t width, size_t height) const
{
    size_t size = width;
    (void)height;

    desktopentry_desktop *desktop = this->_desktops.value(filename, nullptr);
    if (desktop != nullptr) {
        char *path = desktopentry_desktop_get_proper_icon(desktop, size);
        fprintf(stderr, "DesktopEntry::iconPath - (%s) path: %s\n", filename.toStdString().c_str(), path);
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

    // If entry exec is empty, return empty string immediately.
    if (entryExec == "") {
        return QString();
    }

    auto end = this->_desktops.keyValueEnd();
    auto regex_pattern = QRegularExpression("%[a-zA-Z]$");
    for (auto iter = this->_desktops.keyValueBegin(); iter != end; ++iter) {
        auto filename = iter->first;
        auto exec = this->entryExec(filename);
        // Strip desktop entry exec arguments.
        auto idx = exec.indexOf(regex_pattern);
        if (idx != -1) {
            exec.remove(idx, 2);
            exec = exec.trimmed();
        }
        // Strip if double quote contains.
        if (exec.startsWith("\"") && exec.endsWith("\"")) {
            exec.remove(0, 1);
        }
        if (exec.startsWith(entryExec)) {
            return filename;
        }
        // Strip paths so get only compare exec filename.
        exec = fs::path(exec.toStdString()).filename().c_str();
        auto cmp = fs::path(entryExec.toStdString()).filename().c_str();
        if (exec == cmp) {
            return filename;
        }
    }

    return QString();
}

QString DesktopEntry::findFilenameByPath(const QString& path) const
{
    auto end = this->_desktops.keyValueEnd();
    for (auto&& iter = this->_desktops.keyValueBegin(); iter != end; ++iter) {
        const char *iter_path = desktopentry_desktop_path(iter->second);
        if (iter_path == path) {
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
