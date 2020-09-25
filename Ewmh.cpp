#include "Ewmh.h"

#include <stdlib.h>

Ewmh::Ewmh()
{
}

//=======================
// Public functions
//=======================

bool Ewmh::send_net_close_window(uint32_t w)
{
    xcb_connection_t *conn = xcb_connect(NULL, NULL);

    const xcb_setup_t *setup = xcb_get_setup(conn);
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
    xcb_screen_t *screen = iter.data;

    xcb_intern_atom_cookie_t intern_cookie = xcb_intern_atom(conn, 1, strlen("_NET_CLOSE_WINDOW"), "_NET_CLOSE_WINDOW");
    xcb_intern_atom_reply_t *intern_reply = xcb_intern_atom_reply(conn, intern_cookie, NULL);
    xcb_atom_t net_close_window = intern_reply->atom;
    free(intern_reply);

    xcb_client_message_event_t evt;
    evt.response_type = XCB_CLIENT_MESSAGE;
    evt.format = 32;
    evt.window = w;
    evt.type = net_close_window;
    evt.data.data32[0] = 0;
    evt.data.data32[1] = 2;
    evt.data.data32[2] = 0;
    evt.data.data32[3] = 0;
    evt.data.data32[4] = 0;

    auto cookie = Ewmh::send_event(conn,
        screen->root,
        XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
        (const char*)&evt
    );

    xcb_generic_error_t *err = xcb_request_check(conn, cookie);
    if (err) {
        xcb_disconnect(conn);
        return false;
    }

    xcb_disconnect(conn);

    return true;
}

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

uint32_t Ewmh::get_net_wm_pid(uint32_t w)
{
    xcb_connection_t *conn = xcb_connect(NULL, NULL);

    auto cookie = Ewmh::get_property(conn, w, "_NET_WM_PID", XCB_ATOM_CARDINAL);
    xcb_get_property_reply_t *reply = xcb_get_property_reply(conn, cookie, NULL);
    size_t len = xcb_get_property_value_length(reply);
    void *val = xcb_get_property_value(reply);

    uint32_t ret = (len != 0) ? *((uint32_t*)val) : 0;

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

xcb_void_cookie_t Ewmh::send_event(xcb_connection_t *conn,
        xcb_window_t w, uint32_t event_mask, const char *event)
{
    xcb_void_cookie_t cookie = xcb_send_event_checked(conn, 0, w, event_mask, event);
    xcb_flush(conn);

    return cookie;
}
