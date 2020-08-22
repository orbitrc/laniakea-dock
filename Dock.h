#ifndef DOCK_H
#define DOCK_H

#include <QObject>

#include <X11/Xlib.h>

#include "Item.h"

class Dock : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QList<int> windows READ windows NOTIFY windowsChanged)
    Q_PROPERTY(QList<QString> pinnedIds READ pinnedIds NOTIFY pinnedIdsChanged)
public:
    explicit Dock(QObject *parent = nullptr);
    ~Dock();

    QList<int> windows() const;
    QList<QString> pinnedIds() const;

    void appendItem(Item::ItemType type, QString cls, bool pinned = false);

    void list_clients();

private:
    bool is_normal_window(unsigned long w_id) const;
    QString get_wm_class(unsigned long w_id) const;
    unsigned char* get_window_property(unsigned long w_id, const char *prop_name,
            Atom req_type, unsigned long *size) const;

signals:
    void windowsChanged();
    void pinnedIdsChanged();

private:
    QList<int> m_windows;
    QList<Item*> m_items;

    Display *_dpy;
};

#endif // DOCK_H
