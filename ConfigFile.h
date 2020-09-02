#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <optional>

#include <QMap>

#define LA_DOCK_CONFIG_PATH ".config/laniakea/dock.conf"
#define LA_DOCK_CONFIG_PATH_TMP ".config/laniakea/dock.conf.tmp"

class ConfigFile
{
public:
    ConfigFile();

    void load();
    bool save();

    QList<QString> sections() const;

    // Get value by key.
    std::optional<QString> get_string(const QString& section, const QString& key) const;

    // Insert, append, remove.
    bool append_section(const QMap<QString, QString>& values);
    bool insert_section(size_t index, const QMap<QString, QString>& values);

private:
    QMap<QString, QMap<QString, QString>> _config;
};

#endif // CONFIGFILE_H
