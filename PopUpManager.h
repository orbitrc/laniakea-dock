#ifndef POPUPMANAGER_H
#define POPUPMANAGER_H

#include <QObject>

#include <QQmlEngine>

class Dock;

class PopUpManager : public QObject
{
    Q_OBJECT
public:
    explicit PopUpManager(QQmlEngine *engine, Dock *dock, QObject *parent = nullptr);

    Q_INVOKABLE void showContextMenu(const QString& id);
    Q_INVOKABLE void showToolTip(const QString& id);
signals:

private:
    QQmlEngine *_engine;
    Dock *_dock;
};

#endif // POPUPMANAGER_H
