#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <optional>

#include <QMap>

#define LA_DOCK_CONFIG_PATH ".config/laniakea/dock.conf"

class ConfigFile
{
public:
    ConfigFile();

    void load();

    std::optional<QString> get_string(const QString& section, const QString& key) const;

private:
    QMap<QString, QMap<QString, QString>> _config;
};

#endif // CONFIGFILE_H
