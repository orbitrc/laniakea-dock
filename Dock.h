#ifndef DOCK_H
#define DOCK_H

#include <QObject>

#include <X11/Xlib.h>

class Dock : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QList<int> windows READ windows NOTIFY windowsChanged)
public:
    explicit Dock(QObject *parent = nullptr);
    ~Dock();

    QList<int> windows() const;

    void list_clients();

private:
    bool is_normal_window(unsigned long w_id) const;

signals:
    void windowsChanged();

private:
    QList<int> m_windows;

    Display *_dpy;
};

#endif // DOCK_H
