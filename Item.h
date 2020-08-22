#ifndef ITEM_H
#define ITEM_H

#include <QObject>

#include <optional>

class Item : public QObject
{
    Q_OBJECT
public:
    enum class ItemType {
        DesktopEntry,
        AppImage,
        Exec,
    };

public:
    explicit Item(QObject *parent = nullptr);

    QString id() const;
    void setId(const QString& id);

    bool pinned() const;
    void setPinned(bool val);

    ItemType type() const;
    void setType(ItemType type);

    std::optional<QString> cls() const;
    void setCls(const QString& cls);
    void clearCls();

    QList<int> windows() const;
    void appendWindow(int wId);
    void removeWindow(int wId);

signals:

private:
    QString m_id;
    bool m_pinned;
    ItemType m_type;
    std::optional<QString> m_class;
    QList<int> m_wIds;
};

#endif // ITEM_H
