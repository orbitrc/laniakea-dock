#ifndef DOCKWIDGET_H
#define DOCKWIDGET_H

#include <QQuickWidget>

#include <QQmlEngine>

class DockWidget : public QQuickWidget
{
    Q_OBJECT
public:
    DockWidget(QQmlEngine *engine, QWidget *parent);

protected:
    virtual bool event(QEvent *) override;

private:
    void set_on_all_desktop();
    void set_widget_strut();

signals:
    void geometryChanged();
};

#endif // DOCKWIDGET_H
