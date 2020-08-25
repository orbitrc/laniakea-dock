#include "MenuWidget.h"

MenuWidget::MenuWidget(QQmlEngine *engine, QWidget *parent)
    : QQuickWidget(engine, parent)
{
    setAttribute(Qt::WA_X11NetWmWindowTypeMenu);
    setWindowFlag(Qt::FramelessWindowHint);

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_AlwaysStackOnTop);
    setClearColor(Qt::transparent);
}
