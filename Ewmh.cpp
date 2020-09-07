#include "Ewmh.h"

#include <stdlib.h>

Ewmh::Ewmh()
{
}

//=======================
// Public functions
//=======================

QString Ewmh::get_net_wm_name(uint32_t w)
{
    xcb_connection_t *conn = xcb_connect(NULL, NULL);

    auto cookie = Ewmh::get_property(conn, w, "_NET_WM_NAME", XCB_ATOM_STRING);
    xcb_get_property_reply_t *reply = xcb_get_property_reply(conn, cookie, NULL);
    size_t len = xcb_get_property_value_length(reply);
    void *val = xcb_get_property_value(reply);

    QString ret(static_cast<QChar*>(val), len);

    free(reply);
    xcb_disconnect(conn);

    return ret;
}

uint32_t Ewmh::get_net_wm_desktop(uint32_t w)
{
    xcb_connection_t *conn = xcb_connect(NULL, NULL);

    auto cookie = Ewmh::get_property(conn, w, "_NET_WM_DESKTOP", XCB_ATOM_CARDINAL);
    xcb_get_property_reply_t *reply = xcb_get_property_reply(conn, cookie, NULL);
    size_t len = xcb_get_property_value_length(reply);
    void *val = xcb_get_property_value(reply);

    fprintf(stderr, "Ewmh::get_net_wm_desktop - len: %ld\n", len);
    uint32_t ret = *((uint32_t*)val);

    free(reply);
    xcb_disconnect(conn);

    return ret;
}

//=======================
// Private functions
//=======================

xcb_get_property_cookie_t Ewmh::get_property(xcb_connection_t *conn,
        xcb_window_t w, const QString &prop, xcb_atom_t type)
{
    xcb_intern_atom_cookie_t intern_cookie = xcb_intern_atom(
        conn,
        1,
        prop.length(),
        prop.toStdString().c_str()
    );
    xcb_intern_atom_reply_t *intern_reply = xcb_intern_atom_reply(conn, intern_cookie, NULL);
    xcb_atom_t atom_prop = intern_reply->atom;
    free(intern_reply);

    xcb_get_property_cookie_t cookie = xcb_get_property(
        conn,
        0,
        w,
        atom_prop,
        type,
        0,
        1024
    );

    return cookie;
}
