#include "IconImageProvider.h"

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

    pixmap.fill(QColor(id).rgba());

    return pixmap;
}
