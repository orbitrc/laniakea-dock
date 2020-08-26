#include "PopUpManager.h"

#include <QQuickItem>

#include "MenuWidget.h"
#include "ToolTipWidget.h"

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

void PopUpManager::showToolTip(const QString &id)
{
    ToolTipWidget *toolTipWidget = new ToolTipWidget(this->_engine);
    toolTipWidget->setSource(QUrl("qrc:/qml/ToolTip.qml"));
    toolTipWidget->rootObject()->setProperty("itemId", id);

    // Set target rect.
    Item *item = this->_dock->item_by_id(id);
    if (item) {
        toolTipWidget->setTargetRect(item->iconGeometry());
    }

    // Set geometry.
    if (item) {
        int tool_tip_width = toolTipWidget->rootObject()->property("width").toInt();
        int offset_x = ((tool_tip_width / 2) - (item->iconGeometry().width() / 2));
        toolTipWidget->setGeometry(
            item->iconGeometry().x() - offset_x,
            item->iconGeometry().y() - toolTipWidget->height(),
            tool_tip_width, toolTipWidget->height()
        );
    }

    toolTipWidget->show();
}
