#include "Dock.h"

#include <stdio.h>
#include <algorithm>
#include <filesystem>

#include <QPainter>
#include <QThread>

#include <X11/Xatom.h>

#include "ConfigFile.h"

Dock::Dock(QObject *parent)
    : QObject(parent)
{
    this->_dpy = XOpenDisplay(NULL);

    // Connect signals.
    QObject::connect(this, &Dock::itemAdded, this, &Dock::onItemAdded);
    QObject::connect(this, &Dock::activeWindowChanged, this, &Dock::onActiveWindowChanged);
    QObject::connect(this, &Dock::windowAdded, this, &Dock::onWindowAdded);
    QObject::connect(this, &Dock::windowRemoved, this, &Dock::onWindowRemoved);

    // Config file.
    this->_config = new ConfigFile();
    this->_config->load();
    this->list_pinned_items();

    // Desktop entries.
    this->_desktop_entry = new DesktopEntry();

    // Initail properties.
    this->list_clients();
    this->update_active_window();

    // Run workers.
    this->thr_monitor_x = QThread::create([this]() {
        this->monitor_x_events();
    });
    this->thr_monitor_x->start();
}

Dock::~Dock()
{
    // Delete config file.
    delete this->_config;

    // Stop workers.
    this->x_event_monitoring = false;
    this->thr_monitor_x->exit();
    while (this->thr_monitor_x->isRunning()) {
        this->thr_monitor_x->wait(100);
    }

    XCloseDisplay(this->_dpy);
}

//====================
// Property methods
//====================

QList<int> Dock::windows() const
{
    return this->m_windows;
}

QList<QString> Dock::pinnedIds() const
{
    QList<QString> ids;
    for (int i = 0; i < this->m_items.length(); ++i) {
        if (this->m_items[i]->pinned()) {
            ids.append(this->m_items[i]->id());
        }
    }
    return ids;
}

QList<QString> Dock::itemIds() const
{
    QList<QString> ids;
    for (int i = 0; i < this->m_items.length(); ++i) {
        ids.append(this->m_items[i]->id());
    }

    return ids;
}


void Dock::appendItem(Item::ItemType type, QString cls, bool pinned)
{
    Item *item = new Item(type);
    item->setCls(cls);
    item->setPinned(pinned);

    this->m_items.append(item);

    emit this->itemAdded();
}

void Dock::appendItem(Item *item)
{
    this->m_items.append(item);

    emit this->itemAdded();
}

QString Dock::activeWindowItemId() const
{
    Item *item = nullptr;
    for (int i = 0; i < this->m_items.length(); ++i) {
        auto windows = this->m_items[i]->windows();
        for (int j = 0; j < windows.length(); ++j) {
            if (windows[j] == this->m_activeWindow) {
                item = this->m_items[i];
            }
        }
    }

    if (item != nullptr) {
        return item->id();
    }

    return QString();
}

QPixmap Dock::current_window_icon(const QString &id) const
{
    Item *item = nullptr;

    for (int i = 0; i < this->m_items.length(); ++i) {
        if (this->m_items[i]->id() == id) {
            item = this->m_items[i];
            break;
        }
    }

    if (item != nullptr && item->windows().length() > 0) {
        fprintf(stderr, "Dock::current_window_icon - Found item: %s\n", item->id().toStdString().c_str());
        return this->get_window_icon(item->windows()[0], 48);
    }

    QPixmap default_pixmap(1, 1);
    default_pixmap.fill(QColor(0, 255, 0, 100));
    return default_pixmap;
}

QPixmap Dock::item_default_icon(const QString &id) const
{
    namespace fs = std::filesystem;

    Item *item = this->item_by_id(id);

    if (item) {
        fs::path desktop_path = item->path().toStdString();
        if (desktop_path != "") {
            auto desktop_filename = desktop_path.filename();
            auto icon_path = this->_desktop_entry->iconPath(
                desktop_filename.c_str(),
                48, // width
                48  // height
            );
            QPixmap icon_pixmap;
            icon_pixmap.load(icon_path);

            return icon_pixmap;
        }
    }

    QPixmap null_pixmap(1, 1);
    null_pixmap.fill(QColor(255, 0, 0, 100));
    return null_pixmap;
}

//=========================
// Q_INVOKABLE methods
//=========================
QString Dock::itemClassById(const QString& id) const
{
    for (int i = 0; i < this->m_items.length(); ++i) {
        if (this->m_items[i]->id() == id) {
            auto cls = this->m_items[i]->cls();
            if (cls.has_value()) {
                return cls.value();
            } else {
                return QString();
            }
        }
    }

    return QString();
}

QList<int> Dock::itemWindowsById(const QString &id) const
{
    for (int i = 0; i < this->m_items.length(); ++i) {
        if (this->m_items[i]->id() == id) {
            auto windows = this->m_items[i]->windows();
            return windows;
        }
    }

    return QList<int>();
}

QRect Dock::itemIconGeometry(const QString &id) const
{
    Item *item = this->item_by_id(id);
    if (item) {
        return item->iconGeometry();
    }

    return QRect();
}

void Dock::itemSetIconGeometry(const QString &id, const QRect &rect)
{
    Item *item = this->item_by_id(id);
    if (item) {
        item->setIconGeometry(rect);
    }
}

QString Dock::itemProperIconName(const QString& id)
{
    Item *item = nullptr;
    item = this->item_by_id(id);
    if (item != nullptr) {
        switch (item->type()) {
        case Item::ItemType::DesktopEntry: {
            // If item has no path, then fallback to WM_CLASS.
            if (item->path() == "") {
                auto cls = item->cls();
                return cls.has_value() ? cls.value() : "";
            }
            auto filename = this->_desktop_entry->findFilenameByPath(item->path());
            return this->_desktop_entry->entryName(filename);
        }
        case Item::ItemType::AppImage: {
            break;
        }
        case Item::ItemType::Exec: {
            break;
        }
        default:
            break;
        }
    }

    return QString();
}

void Dock::activateWindow(int wId)
{
    this->activate_window(wId);
}

void Dock::debugPrint(const QString &str) const
{
    fprintf(stderr, "DEBUG: %s\n", str.toStdString().c_str());
}

//=======================
// Helper methods
//=======================
Item* Dock::item_by_id(const QString& id) const
{
    Item *item = nullptr;
    for (int i = 0; i < this->m_items.length(); ++i) {
        if (this->m_items[i]->id() == id) {
            item = this->m_items[i];
            break;
        }
    }

    return item;
}

void Dock::list_pinned_items()
{
    auto sections = this->_config->sections();
    for (auto&& section: sections) {
        fprintf(stderr, "section: %s\n", section.toStdString().c_str());
        // Get type.
        Item::ItemType item_type;
        auto type = this->_config->get_string(section, "Type");
        if (type.has_value() && type.value() == "DesktopEntry") {
            item_type = Item::ItemType::DesktopEntry;
        } else if (type.has_value() && type.value() == "AppImage") {
            item_type = Item::ItemType::AppImage;
        } else {
            item_type = Item::ItemType::Exec;
        }
        // Get path.
        auto opt_path = this->_config->get_string(section, "Path");
        QString path = "";
        if (opt_path.has_value()) {
            path = opt_path.value();
        }

        Item *item = new Item(item_type, path);
        item->setPinned(true);
        this->appendItem(item);
    }
}


void Dock::list_clients()
{
    Atom prop;
    Atom ret_type;
    int ret_format;
    unsigned long n_items;
    unsigned long ret_bytes_after;
    unsigned char *ret;
    int result;

    // Get property.
    prop = XInternAtom(this->_dpy, "_NET_CLIENT_LIST", False);

    result = XGetWindowProperty(
        this->_dpy,
        XDefaultRootWindow(this->_dpy),
        prop,
        0,      // long_offset
        1024,   // long_length
        False,
        XA_WINDOW,  // req_type
        &ret_type,      // actual_type_return
        &ret_format,    // actual_format_return
        &n_items,
        &ret_bytes_after,
        &ret
    );

    if (result != Success) {
        fprintf(stderr, "Failed! %d\n", result);
        XFree(ret);
        return;
    }

    QList<int> old_windows = this->m_windows;
    QList<int> new_windows;

    for (unsigned long i = 0; i < n_items; ++i) {
        Window w = ((Window*)ret)[i];
        if (this->is_normal_window(w)) {
            new_windows.append(w);
        }
    }

    // Compare old and new.
    for (int i = 0; i < new_windows.length(); ++i) {
        if (!old_windows.contains(new_windows[i])) {
            // Window is added.
            emit this->windowAdded(new_windows[i]);
        }
    }
    for (int i = 0; i < old_windows.length(); ++i) {
        if (!new_windows.contains(old_windows[i])) {
            // Window is removed.
            emit this->windowRemoved(old_windows[i]);
        }
    }

    this->m_windows = new_windows;

    XFree(ret);
}

//========================
// Private methods
//========================

bool Dock::is_normal_window(unsigned long w_id) const
{
    Atom prop;
    Atom ret_type;
    int ret_format;
    unsigned long n_items;
    unsigned long ret_bytes_after;
    unsigned char *ret;
    int result;
    Atom type_normal;

    // Get property.
    prop = XInternAtom(this->_dpy, "_NET_WM_WINDOW_TYPE", False);

    result = XGetWindowProperty(
        this->_dpy,
        w_id,
        prop,
        0,
        1024,
        False,
        XA_ATOM,
        &ret_type,
        &ret_format,
        &n_items,
        &ret_bytes_after,
        &ret
    );

    if (result != Success) {
        XFree(ret);
        return true;
    }

    type_normal = XInternAtom(this->_dpy, "_NET_WM_WINDOW_TYPE_NORMAL", False);

    for (unsigned long i = 0; i < n_items; ++i) {
        if (((Atom*)ret)[i] == type_normal) {
            XFree(ret);
            return true;
        }
    }

    XFree(ret);
    return false;
}

unsigned char* Dock::get_window_property(unsigned long w_id, const char *prop_name,
        Atom req_type, unsigned long *size) const
{
    Display *dpy;   // To prevent thread blocking.
    Atom prop;
    Atom ret_type;
    int ret_format;
    unsigned long n_items;
    unsigned long ret_bytes_after;
    unsigned char *ret;
    int result;

    dpy = XOpenDisplay(NULL);

    // Get property.
    prop = XInternAtom(dpy, prop_name, False);

    result = XGetWindowProperty(
        dpy,
        w_id,
        prop,
        0,
        4194304,    // 2 ** 22
        False,
        req_type,
        &ret_type,
        &ret_format,
        &n_items,
        &ret_bytes_after,
        &ret
    );

    *size = n_items;

    XCloseDisplay(dpy);

    return ret;
}

QString Dock::get_wm_class(unsigned long w_id) const
{
    unsigned long size;
    unsigned char *ret;
    ret = this->get_window_property(w_id, "WM_CLASS", XA_STRING, &size);

    char *iter = (char*)ret;
    for (; *iter != '\0'; ++iter) {
    }
    ++iter;
    QString result(iter);

    XFree(ret);

    return result;
}

QPixmap Dock::get_window_icon(unsigned long w_id, unsigned long req_size) const
{
    unsigned long size;
    unsigned char *ret;
    ret = this->get_window_property(w_id, "_NET_WM_ICON", XA_CARDINAL, &size);

    fprintf(stderr, "%ld\n", size);
    unsigned long *icon;
    icon = (unsigned long*)ret;

    // Get available icon sizes.
    QList<unsigned long> sizes;
    unsigned long *end = icon + size;
    fprintf(stderr, "sizes: ");
    while (icon != end) {
        unsigned long width = *icon++;
        unsigned long height = *icon++;
        fprintf(stderr, "%ldx%ld ", width, height);
        sizes.append(width);
        if (width >= 1024) {
            break;
        }
        icon += width * height;
    }
    fprintf(stderr, "\n");
    std::sort(sizes.begin(), sizes.end());
    // Get ideal icon size.
    unsigned long ideal_size = 0;
    for (int i = 0; i < sizes.length(); ++i) {
        ideal_size = sizes[i];
        if (ideal_size >= req_size) {
            break;
        }
    }
    fprintf(stderr, "ideal size: %ld\n", ideal_size);
    // Find ideal icon size position.
    icon = (unsigned long*)ret;
    while (true) {
        if (ideal_size == 0) {
            break;
        }
        unsigned long width = *icon++;
        unsigned long height = *icon++;
        if (width == ideal_size) {
            icon -= 2;
            break;
        }
        icon += width * height;
    }
    // Draw pixmap.
    unsigned long width = ideal_size > 0 ? *icon++ : 0;
    unsigned long height = ideal_size > 0 ? *icon++ : 0;
    QPixmap pixmap(ideal_size, ideal_size);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    for (unsigned long i = 0; i < width; ++i) {
        for (unsigned long j = 0; j < height; ++j) {
            unsigned char *argb = (unsigned char*)icon++;
            painter.setPen(QColor(argb[2], argb[1], argb[0], argb[3]));
            painter.drawPoint(j, i);
        }
    }

    XFree(ret);

    return pixmap;
}

unsigned long Dock::get_net_wm_pid(unsigned long w_id) const
{
    unsigned long size;
    unsigned char *ret;
    ret = this->get_window_property(w_id, "_NET_WM_PID", XA_CARDINAL, &size);
    auto pid = *(unsigned long*)ret;
    XFree(ret);

    return pid;
}

Item* Dock::find_item_by_class(const QString &cls)
{
    Item *found = nullptr;
    for (int i = 0; i < this->m_items.length(); ++i) {
        auto item_cls = this->m_items[i]->cls();
        if (item_cls.has_value() && item_cls.value() == cls) {
            found = this->m_items[i];
        }
    }

    return found;
}

Item* Dock::find_item_by_w_id(unsigned long w_id)
{
    Item *found = nullptr;
    for (int i = 0; i < this->m_items.length(); ++i) {
        auto windows = this->m_items[i]->windows();
        if (windows.contains(w_id)) {
            found = this->m_items[i];
        }
    }

    return found;
}

Item* Dock::find_item_by_path(const QString &path)
{
    Item *found = nullptr;
    for (auto& item: this->m_items) {
        if (item->path() == path) {
            found = item;
            break;
        }
    }

    return found;
}

QString Dock::find_exec_path_by_w_id(unsigned long w_id)
{
    namespace fs = std::filesystem;

    auto pid = this->get_net_wm_pid(w_id);
    auto proc_exe = QString("/proc/%1/exe").arg(pid);
    auto symlink_target = fs::read_symlink(proc_exe.toStdString());
    fprintf(stderr, "Dock::find_exec_path_by_w_id - symlink_target: %s\n", symlink_target.c_str());

    return QString(symlink_target.c_str());
}

void Dock::monitor_x_events()
{
    XSetWindowAttributes attrs;
    // Atoms
    Atom atom_net_active_window;

    // Get atoms.
    atom_net_active_window = XInternAtom(this->_dpy, "_NET_ACTIVE_WINDOW", False);

    attrs.event_mask = PropertyChangeMask | SubstructureNotifyMask | StructureNotifyMask;
    XChangeWindowAttributes(this->_dpy, XDefaultRootWindow(this->_dpy), CWEventMask, &attrs);
    this->x_event_monitoring = true;
    while (this->x_event_monitoring) {
        XEvent evt;
        XNextEvent(this->_dpy, &evt);
        // Active window changed.
        if (evt.type == PropertyNotify
                && evt.xproperty.atom == atom_net_active_window) {
            this->update_active_window();
        }
        if (evt.type == ConfigureNotify || evt.type == CreateNotify ||
                evt.type == DestroyNotify) {
            this->list_clients();
        }
    }
    fprintf(stderr, "X event monitoring stopped.\n");
}

void Dock::update_active_window()
{
    unsigned long size;
    unsigned char *ret;

    ret = this->get_window_property(XDefaultRootWindow(this->_dpy),
        "_NET_ACTIVE_WINDOW", XA_WINDOW, &size);

    Window w = ((Window*)ret)[0];
    if ((int)w != this->m_activeWindow) {
        this->m_activeWindow = w;

        emit this->activeWindowChanged();
    }

    XFree(ret);
}

void Dock::activate_window(unsigned long w_id)
{
    XClientMessageEvent evt;
    // Atoms.
    Atom atom_net_active_window;

    atom_net_active_window = XInternAtom(this->_dpy, "_NET_ACTIVE_WINDOW", False);

    evt.type = ClientMessage;
    evt.window = w_id;
    evt.message_type = atom_net_active_window;
    evt.format = 32;
    evt.data.l[0] = 2;  // 2 = pager
    evt.data.l[1] = CurrentTime;    // timestamp
    evt.data.l[2] = 0;
    evt.data.l[3] = 0;
    evt.data.l[4] = 0;

    XSendEvent(
        this->_dpy,
        XDefaultRootWindow(this->_dpy),
        False,
        PropertyChangeMask,
        (XEvent*)&evt
    );
    XFlush(this->_dpy);
}

//===================
// Slots
//===================

void Dock::onItemAdded()
{
    emit this->itemIdsChanged();
}

void Dock::onActiveWindowChanged()
{
    emit this->activeWindowItemIdChanged();
}

void Dock::onWindowAdded(unsigned long wId)
{
    emit this->windowsChanged();

    QString wm_class = this->get_wm_class(wId);

    Item *item = this->find_item_by_class(wm_class);
    QString path;
    if (1) {    // wId is a desktop entry window.
        auto exec_path = this->find_exec_path_by_w_id(wId);
        auto filename = this->_desktop_entry->findFilenameByEntryExec(exec_path);
        path = this->_desktop_entry->path(filename);
        // Find item if only path is exists.
        if (path != "") {
            item = this->find_item_by_path(path);
        }
    }
    // Add an item if not exists yet.
    if (item == nullptr) {
        item = new Item(Item::ItemType::DesktopEntry, path);
        item->setCls(wm_class);
        this->appendItem(item);
        item = this->find_item_by_class(wm_class);
    }
    item->appendWindow(wId);
}

void Dock::onWindowRemoved(unsigned long wId)
{
    emit this->windowsChanged();

    Item *item = this->find_item_by_w_id(wId);
    item->removeWindow(wId);
    // Remove an item if obsolete now.
    if (item->windows().length() == 0 && !item->pinned()) {
        this->m_items.removeOne(item);
        emit this->itemIdsChanged();
    }
}
