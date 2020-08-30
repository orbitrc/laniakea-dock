#include "DockWidget.h"

#include <QQuickItem>
#include <QScreen>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#ifdef Status
#undef Status
#endif

DockWidget::DockWidget(QQmlEngine *engine, QWidget *parent)
    : QQuickWidget(engine, parent)
{
    this->set_on_all_desktop();

    /*
    QObject::connect(this, &QQuickWidget::statusChanged, this, [this](QQuickWidget::Status status) {
        if (status == QQuickWidget::Ready) {
            int h = rootObject()->property("height").toInt();
            int y = screen()->geometry().height() - h;
            setGeometry(0, y, width(), height());
        }
    });
    */
    setGeometry(0, 360, width(), height());
}

//===================
// Event handlers
//===================

bool DockWidget::event(QEvent *evt)
{
    if (evt->type() == QEvent::Paint) {
        emit this->geometryChanged();
    }

    return QQuickWidget::event(evt);
}

//===================
// Private methods
//===================
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
