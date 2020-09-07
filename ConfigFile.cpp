#include "ConfigFile.h"

#include <stdlib.h>
#include <algorithm>
#include <filesystem>
#include <fstream>

#include <QFile>
#include <QTextStream>

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
    auto config_path = fs::path(getenv("HOME"));
    config_path /= LA_DOCK_CONFIG_PATH;
    QFile f(config_path.c_str());
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        fprintf(stderr, "FILE ERROR: %s\n", f.errorString().toStdString().c_str());
    }

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
            section = line.trimmed().replace("[", "").replace("]", "");
            this->_config[section] = QMap<QString, QString>();
            continue;
        }
        // Key-value pair.
        if (line.contains("=")) {
            auto kv = line.trimmed().split('=');
            this->_config[section][kv[0].trimmed()] = kv[1].trimmed();
        }
    }
    f.close();
}

bool ConfigFile::save()
{
    auto keys = this->sections();
    std::sort(keys.begin(), keys.end());

    // Open temporary file.
    auto tmp_path = fs::path(getenv("HOME")) / LA_DOCK_CONFIG_PATH_TMP;
    QFile f_tmp(tmp_path.c_str());
    if (!f_tmp.open(QIODevice::WriteOnly | QIODevice::Text)) {
        fprintf(stderr, "FILE ERROR: %s\n", f_tmp.errorString().toStdString().c_str());
        f_tmp.close();
        return false;
    }
    QTextStream stream(&f_tmp);

    // Write contents to file.
    for (auto&& section: keys) {
        // Write section.
        auto section_values = this->_config.value(section);
        stream << QString("[%1]").arg(section);
        stream << "\n";
        // Write type.
        auto type = this->get_string(section, "Type");
        if (!type.has_value()) {
            f_tmp.remove();
            return false;
        }
        stream << QString("Type=") + type.value();
        stream << "\n";
        // Write path.
        auto path = this->get_string(section, "Path");
        if (!path.has_value()) {
            f_tmp.remove();
            return false;
        }
        stream << QString("Path=") + path.value();
        stream << "\n\n";
    }
    f_tmp.close();

    // Copy tmp file to real config file.
    auto path = fs::path(getenv("HOME")) / LA_DOCK_CONFIG_PATH;
    QFile::remove(path.c_str());
    auto copied = f_tmp.copy(path.c_str());
    f_tmp.remove();
    if (!copied) {
        return false;
    }

    return true;
}

QList<QString> ConfigFile::sections() const
{
    return this->_config.keys();
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

//==========================
// Insert, append, remove
//==========================

bool ConfigFile::append_section(const QMap<QString, QString> &values)
{
    // Find last index.
    auto idx = this->sections().length();

    // Make section name.
    QString section = QString("Item %1").arg(idx);

    // Validate values.
    if (!values.contains("Type")) {
        return false;
    }
    if (!values.contains("Path")) {
        return false;
    }

    // Insert.
    this->_config.insert(section, values);

    return true;
}

bool ConfigFile::insert_section(size_t index, const QMap<QString, QString> &values)
{
    // TODO: Implement.
    #warning "ConfigFile::insert_section not implemented"
    (void)index;
    (void)values;

    return false;
}

bool ConfigFile::remove_section(size_t index)
{
    // Bound check.
    if (static_cast<size_t>(this->_config.keys().length()) - 1 > index) {
        return false;
    }

    // Remove.
    QString section = QString("Item %1").arg(index + 1);
    this->_config.remove(section);

    // Re arange sections.
    auto keys = this->_config.keys();
    std::sort(keys.begin(), keys.end());
    for (int i = index; i <= keys.length(); ++i) {
        QString section_name = QString("Item %1").arg(i + 1);
        QString new_section_name = QString("Item %1").arg(i);
        auto values = this->_config.value(section_name);
        this->_config.remove(section_name);
        this->_config.insert(new_section_name, values);
    }

    return true;
}
