#ifndef DOCK_MENUWIDGET_H
#define DOCK_MENUWIDGET_H

#include <QQuickWidget>

#include <QQmlEngine>

class MenuWidget : public QQuickWidget
{
public:
    MenuWidget(QQmlEngine *engine, QWidget *parent = nullptr);
    ~MenuWidget();

protected:
    virtual bool event(QEvent *) override;
    virtual void mousePressEvent(QMouseEvent *) override;
};

#endif // DOCK_MENUWIDGET_H
