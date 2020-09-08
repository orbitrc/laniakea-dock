#ifndef _LA_DOCK_EWMH_H
#define _LA_DOCK_EWMH_H

#include <QString>

#include <stdint.h>

#include <xcb/xcb.h>

class Ewmh
{
public:
    Ewmh();

    //=================================
    // Other root window messages
    //=================================
    static bool send_net_close_window(uint32_t w);

    //=================================
    // Application window properties
    //=================================
    static QString get_net_wm_name(uint32_t w);
    static uint32_t get_net_wm_desktop(uint32_t w);

private:
    static xcb_get_property_cookie_t get_property(xcb_connection_t *conn,
            xcb_window_t w, const QString& prop, xcb_atom_t type);

    static xcb_void_cookie_t send_event(xcb_connection_t *conn,
            xcb_window_t w, uint32_t event_mask, const char *event);
};

#endif // _LA_DOCK_EWMH_H
