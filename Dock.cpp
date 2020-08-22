#include "Dock.h"

#include <stdio.h>

#include <X11/Xatom.h>

Dock::Dock(QObject *parent)
    : QObject(parent)
{
    this->_dpy = XOpenDisplay(NULL);

    // Connect signals.
    QObject::connect(this, &Dock::itemAdded, this, &Dock::onItemAdded);

    this->list_clients();
}

Dock::~Dock()
{
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

    if (static_cast<unsigned long>(this->m_windows.length()) != n_items) {
//        n_windows = n_items;
//        printf("n_items: %d\n", n_items);
        for (unsigned long i = 0; i < n_items; ++i) {
//            printf("0x%08x\n", ((Window*)ret)[i]);
            if (this->is_normal_window(((Window*)ret)[i])) {
                this->m_windows.append(((Window*)ret)[i]);
                QString wm_class = this->get_wm_class(((Window*)ret)[i]);
                fprintf(stderr, "%s\n", wm_class.toStdString().c_str());
                // Add item if not exists yet.
                Item *item = this->find_item_by_class(wm_class);
                if (item == nullptr) {
                    this->appendItem(Item::ItemType::DesktopEntry, wm_class);
                    item = this->find_item_by_class(wm_class);
                }
            }
        }
        emit this->windowsChanged();
    }

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

//===================
// Slots
//===================

void Dock::onItemAdded()
{
    emit this->itemIdsChanged();
}
