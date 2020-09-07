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

    // If id is item id.
    if (this->is_item_id(id)) {
        Item *item = this->_dock->item_by_id(id);
        if (item) {
            return this->_dock->item_default_icon(id);
        } else {
            pixmap.fill(QColor(255, 0, 0, 100));
            return pixmap;
        }
    }

    // If id is window id.
    QPixmap p = this->_dock->window_icon(id.toInt());
    return p;
}

bool IconImageProvider::is_item_id(const QString &val) const
{
    if (val.length() == 36 && val[8] == '-') {
        return true;
    }

    return false;
}
