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
    Dock *_dock;
};

#endif // ICONIMAGEPROVIDER_H
