#ifndef ICONIMAGEPROVIDER_H
#define ICONIMAGEPROVIDER_H

#include <QQuickImageProvider>

class Dock;

class IconImageProvider : public QQuickImageProvider
{
public:
    IconImageProvider(Dock *dock);

    virtual QPixmap requestPixmap(const QString& id, QSize *size, const QSize& requestedSize) override;

private:
    /// If val is item id(UUID) then return true, or if val is window id return false.
    bool is_item_id(const QString& val) const;
private:
    Dock *_dock;
};

#endif // ICONIMAGEPROVIDER_H
