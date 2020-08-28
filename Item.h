#ifndef ITEM_H
#define ITEM_H

#include <QObject>

#include <optional>

#include <QRect>

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

    QRect iconGeometry() const;
    void setIconGeometry(const QRect& rect);

signals:
    void iconGeometryChanged(const QRect& rect);

public slots:
    void changeNetWmIconGeometry(const QRect& rect);

private:
    QString m_id;
    bool m_pinned;
    ItemType m_type;
    std::optional<QString> m_class;
    QList<int> m_wIds;
    QRect m_iconGeometry;
};

#endif // ITEM_H
