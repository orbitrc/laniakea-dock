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

    // Set geometry.
    Item *item = this->_dock->item_by_id(id);
    if (item) {
        int offset_x = ((menuWidget->width() / 2) - (item->iconGeometry().width() / 2));
        menuWidget->setGeometry(
            item->iconGeometry().x() - offset_x,
            item->iconGeometry().y() - menuWidget->height(),
            menuWidget->width(), menuWidget->height()
        );
    }

    menuWidget->show();
}
