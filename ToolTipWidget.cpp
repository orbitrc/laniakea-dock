#include "ToolTipWidget.h"

#include <QMouseEvent>

ToolTipWidget::ToolTipWidget(QQmlEngine *engine, QWidget *parent)
    : QQuickWidget(engine, parent)
{
    setAttribute(Qt::WA_X11NetWmWindowTypeToolTip);
    setWindowFlag(Qt::ToolTip);
    setWindowFlag(Qt::NoDropShadowWindowHint);
    setWindowFlag(Qt::FramelessWindowHint);

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_AlwaysStackOnTop);
    setClearColor(Qt::transparent);
}

void ToolTipWidget::setTargetRect(const QRect &rect)
{
    this->m_targetRect = rect;
}

//=======================
// Event handlers
//=======================

void ToolTipWidget::mouseMoveEvent(QMouseEvent *evt)
{
    if (!this->m_targetRect.contains(evt->globalX(), evt->globalY())) {
        close();
        deleteLater();
    }
}

bool ToolTipWidget::event(QEvent *evt)
{
    if (evt->type() == QEvent::Paint) {
        QWindow *window = windowHandle();
        if (window) {
            window->setMouseGrabEnabled(true);
        }
    }

    return QQuickWidget::event(evt);
}
