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
    DesktopEntry();
    ~DesktopEntry();

    void init();

    /// \brief TODO: Add comment.
    QString path(const QString& filename) const;

    /// \brief Get the value for key Name in [Entry] section by filename.
    ///
    /// \param filename The filename of .desktop file.
    /// \return The value for key Name.
    QString entryName(const QString& filename) const;

    /// \brief Get the value for key Exec in [Entry] section by filename.
    ///
    /// \param filename The filename of .desktop file.
    /// \return The value for key Exec.
    QString entryExec(const QString& filename) const;

    /// \brief Get the value for key Icon in [Entry] section by filename.
    ///
    /// \param filename The filename of .desktop file.
    /// \return The value for key Icon.
    QString entryIcon(const QString& filename) const;

    /// \brief Get the proper sized icon path by filename.
    ///
    /// \param filename The filename of .desktop file.
    /// \param width The width of icon want to get.
    /// \param height The height of icon want to get.
    /// \return Found icon path or return empty string.
    QString iconPath(const QString& filename, size_t width, size_t height) const;

    //======================
    // Find methods
    //======================
    /// @brief Find filename by given entry exec.
    ///
    /// @param entryExec The key that find by.
    ///
    /// @return Filename of desktop. If not exists then returns empty string.
    QString findFilenameByEntryExec(const QString& entryExec) const;

    /// @brief Find filename by given .desktop file path.
    ///
    /// @param path The path that find by.
    ///
    /// @return Filename of desktop. If not exists then returns empty string.
    QString findFilenameByPath(const QString& path) const;

private:
    //======================
    // Helper functions
    //======================
    static bool ends_with(const char *str, const char *cmp);

private:
    /// <File name, desktopentry_desktop*> key=value pairs.
    QMap<QString, desktopentry_desktop*> _desktops;
};

#endif // DESKTOPENTRY_H
