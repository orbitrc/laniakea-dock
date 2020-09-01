#ifndef ITEM_H
#define ITEM_H

#include <QObject>

#include <optional>

#include <QRect>

#include "DesktopEntry.h"

class Item : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
public:
    enum class ItemType {
        DesktopEntry,
        AppImage,
        Exec,
    };

public:
    explicit Item(ItemType type, const QString& path = "", QObject *parent = nullptr);
    ~Item();

    QString id() const;
    void setId(const QString& id);

    bool pinned() const;
    void setPinned(bool val);

    ItemType type() const;
    void setType(ItemType type);

    std::optional<QString> cls() const;
    void setCls(const QString& cls);
    void clearCls();

    QString path() const;
    void setPath(const QString& path);

    QList<int> windows() const;
    void appendWindow(int wId);
    void removeWindow(int wId);

    QRect iconGeometry() const;
    void setIconGeometry(const QRect& rect);

signals:
    void idChanged(const QString& id);

    void iconGeometryChanged(const QRect& rect);

public slots:
    void changeNetWmIconGeometry(const QRect& rect);

private:
    QString m_id;
    bool m_pinned;
    ItemType m_type;
    std::optional<QString> m_class;
    QString m_path;
    QList<int> m_wIds;
    QRect m_iconGeometry;

    void *_appimage;
    void *_exec;
};

#endif // ITEM_H
