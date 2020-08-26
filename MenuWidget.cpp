#include "MenuWidget.h"

#include <stdio.h>

#include <QMouseEvent>

MenuWidget::MenuWidget(QQmlEngine *engine, QWidget *parent)
    : QQuickWidget(engine, parent)
{
    setAttribute(Qt::WA_X11NetWmWindowTypeMenu);
    setWindowFlag(Qt::FramelessWindowHint);

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_AlwaysStackOnTop);
    setClearColor(Qt::transparent);
}

MenuWidget::~MenuWidget()
{
    fprintf(stderr, "MenuWidget destroy\n");
}

//========================
// Event handlers
//========================

bool MenuWidget::event(QEvent *evt)
{
    if (evt->type() == QEvent::Paint) {
        // Grab mouse to menu window.
        QWindow *window = windowHandle();
        if (window) {
            window->setMouseGrabEnabled(true);
        }
    }

    return QQuickWidget::event(evt);
}

void MenuWidget::mousePressEvent(QMouseEvent *evt)
{
    fprintf(stderr, "Mouse pressed.\n");
    releaseMouse();
    QWindow *window = windowHandle();
    if (window) {
        window->setMouseGrabEnabled(false);
    }

    close();

    QQuickWidget::mousePressEvent(evt);
    deleteLater();
}
