#include "DockWidget.h"

#include <QGuiApplication>
#include <QQuickItem>
#include <QScreen>

#include <xcb/xcb.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#ifdef Status
#undef Status
#endif

DockWidget::DockWidget(QQmlEngine *engine, QWidget *parent)
    : QQuickWidget(engine, parent)
{
    this->set_on_all_desktop();

    QObject::connect(this, &QQuickWidget::statusChanged, this, [this](QQuickWidget::Status status) {
        if (status == QQuickWidget::Ready) {
            int h = rootObject()->property("height").toInt();
            auto pos = this->primary_screen_position(h);
            setGeometry(pos.x(), pos.y(), width(), height());
        }
    });
}

//===================
// Event handlers
//===================

bool DockWidget::event(QEvent *evt)
{
    switch (evt->type()) {
    case QEvent::Paint:
        emit this->geometryChanged();
        break;
    case QEvent::WinIdChange:
        this->set_widget_strut();
        break;
    default:
        break;
    }

    return QQuickWidget::event(evt);
}

//===================
// Private methods
//===================
QPoint DockWidget::primary_screen_position(int height) const
{
    auto primaryScreen = QGuiApplication::primaryScreen();
    int x = primaryScreen->geometry().x();
    int y = (primaryScreen->geometry().y() + primaryScreen->geometry().height()) - height;

    return QPoint(x, y);
}

void DockWidget::set_on_all_desktop()
{
    Display *dpy;
    // Atoms.
    Atom atom_net_wm_desktop;

    dpy = XOpenDisplay(NULL);

    atom_net_wm_desktop = XInternAtom(dpy, "_NET_WM_DESKTOP", False);

    unsigned long data = 0xFFFFFFFF;
    XChangeProperty(
        dpy,
        winId(),
        atom_net_wm_desktop,
        XA_CARDINAL,
        32,
        PropModeReplace,
        (unsigned char*)(&data),
        1
    );

    XCloseDisplay(dpy);
}

// This function is duck taped, not perfect. Fix this later or migrate to
// another function.
static uint32_t lowest_bottom_y()
{
    QScreen *lowest_screen = QGuiApplication::primaryScreen();

    auto screens = QGuiApplication::screens();
    for (auto&& screen: screens) {
        QScreen *cmp = screen;
        QRect cmp_geo = cmp->geometry();
        if (cmp_geo.y() + cmp_geo.height() > lowest_screen->geometry().y() + lowest_screen->geometry().height()) {
            lowest_screen = screen;
        }
    }

    // If primary screen is lowest positioned, just return 0.
    if (lowest_screen == QGuiApplication::primaryScreen()) {
        return 0;
    }

    return lowest_screen->geometry().height();
}

void DockWidget::set_widget_strut()
{
    xcb_connection_t *conn;

    // Connect.
    conn = xcb_connect(NULL, NULL);

    // Get atoms.
    xcb_atom_t net_wm_strut_partial;
    xcb_intern_atom_cookie_t intern_cookie = xcb_intern_atom(
        conn, 1, strlen("_NET_WM_STRUT_PARTIAL"), "_NET_WM_STRUT_PARTIAL");
    xcb_intern_atom_reply_t *intern_reply = xcb_intern_atom_reply(conn, intern_cookie, NULL);
    net_wm_strut_partial = intern_reply->atom;
    free(intern_reply);

    // Get bottom position;
    auto bottom = lowest_bottom_y() + 64;

    // Send the message.
    uint32_t data[12];
    data[0] = 0; data[1] = 0;         // left, right
    data[2] = 0; data[3] = bottom;     // top, bottom
    data[4] = 0; data[5] = 0;           // left_start_y, left_end_y
    data[6] = 0; data[7] = 0;           // right_start_y, right_end_y
    data[8] = 0; data[9] = 0;           // top_start_x, top_end_x
    data[10] = 0; data[11] = 1000;         // bottom_start_x, bottom_end_x

    xcb_void_cookie_t cookie = xcb_change_property_checked(
        conn,
        XCB_PROP_MODE_REPLACE,
        winId(),
        net_wm_strut_partial,
        XCB_ATOM_CARDINAL,
        32,
        12,
        &data
    );
    xcb_flush(conn);

    xcb_generic_error_t *err = xcb_request_check(conn, cookie);
    if (err) {
        fprintf(stderr, "DockWidget::set_widget_strut - error.\n");
    }

    // Disconnect.
    xcb_disconnect(conn);
}
