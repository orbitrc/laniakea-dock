#include "PopUpManager.h"

#include <QQuickItem>

#include "MenuWidget.h"

#include "Dock.h"

PopUpManager::PopUpManager(QQmlEngine *engine, Dock *dock, QObject *parent)
    : QObject(parent)
{
    this->_engine = engine;
    this->_dock = dock;
}

//=========================
// Q_INVOKABLE methods
//=========================
void PopUpManager::showContextMenu(const QString &id)
{
    MenuWidget *menuWidget = new MenuWidget(this->_engine);
    menuWidget->setSource(QUrl("qrc:/qml/ContextMenu.qml"));
    menuWidget->rootObject()->setProperty("itemId", id);

    menuWidget->show();
}
