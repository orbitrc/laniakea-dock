#include "Dock.h"

#include <stdio.h>

#include <X11/Xatom.h>

Dock::Dock(QObject *parent)
    : QObject(parent)
{
    this->_dpy = XOpenDisplay(NULL);

    this->list_clients();
}

Dock::~Dock()
{
    XCloseDisplay(this->_dpy);
}

QList<int> Dock::windows() const
{
    return this->m_windows;
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
            }
        }
        emit this->windowsChanged();
    }

    XFree(ret);
}

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
