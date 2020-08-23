#ifndef DOCKWIDGET_H
#define DOCKWIDGET_H

#include <QQuickWidget>

#include <QQmlEngine>

class DockWidget : public QQuickWidget
{
public:
    DockWidget(QQmlEngine *engine, QWidget *parent);

private:
    void set_on_all_desktop();
};

#endif // DOCKWIDGET_H
