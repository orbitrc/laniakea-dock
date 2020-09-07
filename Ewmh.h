#ifndef _LA_DOCK_EWMH_H
#define _LA_DOCK_EWMH_H

#include <QString>

#include <xcb/xcb.h>

class Ewmh
{
public:
    Ewmh();

    static QString get_net_wm_name(uint32_t w);

private:
    static xcb_get_property_cookie_t get_property(xcb_connection_t *conn,
            xcb_window_t w, const QString& prop, xcb_atom_t type);
};

#endif // _LA_DOCK_EWMH_H
