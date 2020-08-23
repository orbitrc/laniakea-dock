#include "Dock.h"

#include <stdio.h>

#include <QThread>

#include <X11/Xatom.h>

Dock::Dock(QObject *parent)
    : QObject(parent)
{
    this->_dpy = XOpenDisplay(NULL);

    // Connect signals.
    QObject::connect(this, &Dock::itemAdded, this, &Dock::onItemAdded);
    QObject::connect(this, &Dock::activeWindowChanged, this, &Dock::onActiveWindowChanged);
    QObject::connect(this, &Dock::windowAdded, this, &Dock::onWindowAdded);
    QObject::connect(this, &Dock::windowRemoved, this, &Dock::onWindowRemoved);

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
    this->x_event_monitoring = false;
    this->thr_monitor_x->exit();
    while (this->thr_monitor_x->isRunning()) {
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
    Item *item = new Item;
    item->setType(type);
    item->setCls(cls);
    item->setPinned(pinned);

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

void Dock::activateWindow(int wId)
{
    this->activate_window(wId);
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
    Atom prop;
    Atom ret_type;
    int ret_format;
    unsigned long n_items;
    unsigned long ret_bytes_after;
    unsigned char *ret;
    int result;

    // Get property.
    prop = XInternAtom(this->_dpy, prop_name, False);

    result = XGetWindowProperty(
        this->_dpy,
        w_id,
        prop,
        0,
        1024,
        False,
        req_type,
        &ret_type,
        &ret_format,
        &n_items,
        &ret_bytes_after,
        &ret
    );

    *size = n_items;

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
    // Add an item if not exists yet.
    if (item == nullptr) {
        this->appendItem(Item::ItemType::DesktopEntry, wm_class);
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
