#ifndef DOCK_MENUWIDGET_H
#define DOCK_MENUWIDGET_H

#include <QQuickWidget>

#include <QQmlEngine>

class MenuWidget : public QQuickWidget
{
public:
    MenuWidget(QQmlEngine *engine, QWidget *parent = nullptr);
};

#endif // DOCK_MENUWIDGET_H
