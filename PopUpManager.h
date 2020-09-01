#ifndef POPUPMANAGER_H
#define POPUPMANAGER_H

#include <QObject>

#include <QQmlEngine>

class Dock;

class Item;
class ToolTipWidget;

class PopUpManager : public QObject
{
    Q_OBJECT
public:
    explicit PopUpManager(QQmlEngine *engine, Dock *dock, QObject *parent = nullptr);

    Q_INVOKABLE void showContextMenu(Item *item);
    Q_INVOKABLE void showContextMenuById(const QString& id);
    Q_INVOKABLE void showToolTip(const QString& id);
    Q_INVOKABLE void hideToolTip();
signals:

private:
    QQmlEngine *_engine;
    Dock *_dock;

    ToolTipWidget *_tool_tip;
};

#endif // POPUPMANAGER_H
