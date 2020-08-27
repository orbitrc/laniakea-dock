#include "ConfigFile.h"

#include <stdlib.h>
#include <filesystem>
#include <fstream>

#include <QFile>

namespace fs = std::filesystem;

ConfigFile::ConfigFile()
{
    auto config_path = fs::path(getenv("HOME"));
    config_path /= LA_DOCK_CONFIG_PATH;

    // If ~/.config/laniakea not exitst then create.
    if (!fs::exists(config_path.parent_path())) {
        fs::create_directories(config_path.parent_path());
    }

    // Create dock.conf if not exists.
    std::fstream config_file;
    config_file.open(config_path, std::fstream::in | std::fstream::out | std::fstream::app);
    if (!config_file) {
        config_file.open(config_path, std::fstream::in | std::fstream::out | std::fstream::trunc);
    }
    config_file.close();
}

void ConfigFile::load()
{
    QFile f(LA_DOCK_CONFIG_PATH);
    f.open(QIODevice::ReadOnly, QIODevice::Text);
    QByteArray section = "";
    while (!f.atEnd()) {
        QByteArray line = f.readLine();
        // Discard empty line.
        if (line.trimmed() == "") {
            continue;
        }
        // Discard comment line.
        if (line.trimmed().startsWith("#") || line.trimmed().startsWith(";")) {
            continue;
        }
        // Section.
        if (line.trimmed().startsWith("[")) {
            section = line.replace("[", "").replace("]", "");
            this->_config[section] = QMap<QString, QString>();
            continue;
        }
        // Key-value pair.
        if (line.contains("=")) {
            auto kv = line.trimmed().split('=');
            this->_config[section][kv[0]] = kv[1];
        }
    }
    f.close();
}

//=========================
// Get property
//=========================

std::optional<QString> ConfigFile::get_string(const QString& section, const QString& key) const
{
    if (this->_config.contains(section)) {
        return this->_config[section].value(key, "");
    } else {
        return std::optional<QString>();
    }
}
