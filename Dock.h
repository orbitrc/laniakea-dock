#ifndef DOCK_H
#define DOCK_H

#include <QObject>

#include <QPixmap>
#include <QRect>

#include <X11/Xlib.h>

#include "Item.h"

class QThread;

class ConfigFile;
class DesktopEntry;

#define LA_DOCK_EWMH_NET_WM_ICON "_NET_WM_ICON"

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
    void appendItem(Item *item);

    Q_INVOKABLE QString itemClassById(const QString& id) const;
    Q_INVOKABLE QList<int> itemWindowsById(const QString& id) const;
    Q_INVOKABLE QRect itemIconGeometry(const QString& id) const;
    /// @brief Set the item's icon geometry.
    ///
    /// @param id The item's id.
    /// @param rect Rectangle geometry will be changed.
    Q_INVOKABLE void itemSetIconGeometry(const QString& id, const QRect& rect);
    /// @brief Get proper icon name. Depends on system locale or item type.
    ///
    /// @param id The item's id.
    /// @return Found proper name.
    Q_INVOKABLE QString itemProperIconName(const QString& id);
    Q_INVOKABLE void activateWindow(int wId);
    /// @brief Run application.
    ///
    /// @param id The item's id.
    Q_INVOKABLE void runApplication(const QString& id);
    Q_INVOKABLE void debugPrint(const QString& str) const;

    QPixmap current_window_icon(const QString& id) const;
    QPixmap item_default_icon(const QString& id) const;

    //=====================
    // Helper methods
    //=====================
    Item* item_by_id(const QString& id) const;

    void list_clients();

private:
    //=======================
    // X property methods
    //=======================
    unsigned char* get_window_property(unsigned long w_id, const char *prop_name,
            Atom req_type, unsigned long *size) const;
    bool is_normal_window(unsigned long w_id) const;
    QString get_wm_class(unsigned long w_id) const;
    QPixmap get_window_icon(unsigned long w_id, unsigned long req_size) const;
    unsigned long get_net_wm_pid(unsigned long w_id) const;
    void update_active_window();
    void dummy_event();

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
    /// @brief Find an item by given path.
    ///
    /// @param path Path to find.
    ///
    /// @return The item or nullptr.
    Item* find_item_by_path(const QString& path);
    /// @brief Find full exec path by window id.
    ///
    /// e.g. Give 0x04200003 to get "/usr/lib/firefox/firefox".
    ///
    /// @param w_id The window id.
    ///
    /// @return Found full path of exec.
    QString find_exec_path_by_w_id(unsigned long w_id);

    void list_pinned_items();

signals:
    void windowsChanged();
    void windowAdded(unsigned long wId);
    void windowRemoved(unsigned long wId);
    void pinnedIdsChanged();
    void itemIdsChanged();
    void itemAdded();
    void activeWindowChanged();
    void activeWindowItemIdChanged();

    void widgetGeometryChanged();

public slots:
    void onItemAdded();
    void onActiveWindowChanged();
    void onWindowAdded(unsigned long wId);
    void onWindowRemoved(unsigned long wId);

private:
    QList<int> m_windows;
    QList<Item*> m_items;
    int m_activeWindow;

    ConfigFile *_config;
    DesktopEntry *_desktop_entry;

    Display *_dpy;

    bool x_event_monitoring;
    QThread *thr_monitor_x;
};

#endif // DOCK_H
