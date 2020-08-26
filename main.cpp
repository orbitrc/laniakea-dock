#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QQuickWidget>

#include "DockWidget.h"
#include "Dock.h"
#include "IconImageProvider.h"
#include "PopUpManager.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
    QCoreApplication::setApplicationName("Dock");

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    // Dock instance.
    Dock dock;
    engine.rootContext()->setContextProperty("Dock", QVariant::fromValue(&dock));

    // Icon image provider.
    IconImageProvider *iconImageProvider = new IconImageProvider(&dock);
    engine.addImageProvider(QLatin1String("icons"), iconImageProvider);

    // Pop up manager.
    PopUpManager popUpManager(&engine, &dock);
    engine.rootContext()->setContextProperty("PopUpManager", QVariant::fromValue(&popUpManager));

    // Dock widget.
    DockWidget widget(&engine, nullptr);
    widget.setAttribute(Qt::WA_X11NetWmWindowTypeDock);
    widget.setSource(url);

    widget.show();

    return app.exec();
}
