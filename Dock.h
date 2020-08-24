#ifndef DOCK_H
#define DOCK_H

#include <QObject>

#include <QPixmap>

#include <X11/Xlib.h>

#include "Item.h"

class QThread;

class Dock : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QList<int> windows READ windows NOTIFY windowsChanged)
    Q_PROPERTY(QList<QString> pinnedIds READ pinnedIds NOTIFY pinnedIdsChanged)
    Q_PROPERTY(QList<QString> itemIds READ itemIds NOTIFY itemIdsChanged)
    Q_PROPERTY(QString activeWindowItemId READ activeWindowItemId NOTIFY activeWindowItemIdChanged)
public:
    explicit Dock(QObject *parent = nullptr);
    ~Dock();

    //=====================
    // Property methods
    //=====================
    QList<int> windows() const;
    QList<QString> pinnedIds() const;
    QList<QString> itemIds() const;
    QString activeWindowItemId() const;

    void appendItem(Item::ItemType type, QString cls, bool pinned = false);

    Q_INVOKABLE QString itemClassById(const QString& id) const;
    Q_INVOKABLE QList<int> itemWindowsById(const QString& id) const;
    Q_INVOKABLE void activateWindow(int wId);
    Q_INVOKABLE void debugPrint(const QString& str) const;

    QPixmap current_window_icon(const QString& id) const;

    void list_clients();

private:
    //=======================
    // X property methods
    //=======================
    unsigned char* get_window_property(unsigned long w_id, const char *prop_name,
            Atom req_type, unsigned long *size) const;
    bool is_normal_window(unsigned long w_id) const;
    QString get_wm_class(unsigned long w_id) const;
    QPixmap get_window_icon(unsigned long w_id) const;
    void update_active_window();

    void activate_window(unsigned long w_id);

    //======================
    // Monitor methods
    //======================
    void monitor_x_events();

    //======================
    // Helper methods
    //======================
    Item* find_item_by_class(const QString& cls);
    Item* find_item_by_w_id(unsigned long w_id);

signals:
    void windowsChanged();
    void windowAdded(unsigned long wId);
    void windowRemoved(unsigned long wId);
    void pinnedIdsChanged();
    void itemIdsChanged();
    void itemAdded();
    void activeWindowChanged();
    void activeWindowItemIdChanged();

public slots:
    void onItemAdded();
    void onActiveWindowChanged();
    void onWindowAdded(unsigned long wId);
    void onWindowRemoved(unsigned long wId);

private:
    QList<int> m_windows;
    QList<Item*> m_items;
    int m_activeWindow;

    Display *_dpy;

    bool x_event_monitoring;
    QThread *thr_monitor_x;
};

#endif // DOCK_H
