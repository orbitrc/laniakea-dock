#include "IconImageProvider.h"

#include "Dock.h"

IconImageProvider::IconImageProvider(Dock *dock)
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
    this->_dock = dock;
}

QPixmap IconImageProvider::requestPixmap(const QString& id, QSize *size, const QSize& requestedSize)
{
    int width = 48;
    int height = 48;

    if (size) {
        *size = QSize(width, height);
    }
    QPixmap pixmap(
        requestedSize.width() > 0 ? requestedSize.width() : width,
        requestedSize.height() > 0 ? requestedSize.height() : height
    );

    // If no windows for item.
    Item *item = this->_dock->item_by_id(id);
    if (item && item->windows().length() == 0) {
        return this->_dock->item_default_icon(id);
    }

    fprintf(stderr, "IconImageProvider::requestPixmap - id: %s\n", id.toStdString().c_str());
    QPixmap p = this->_dock->current_window_icon(id);

    return p;
}
