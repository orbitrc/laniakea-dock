#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QQuickWidget>

#include "DockWidget.h"
#include "Dock.h"
#include "IconImageProvider.h"
#include "PopUpManager.h"

#define LA_DOCK_VERSION "0.1.0"

int main(int argc, char *argv[])
{
    // Shell arguments processing.
    for (int i = 0; i < argc; ++i) {
        QString arg = argv[i];
        if (arg == "--version") {
            fprintf(stdout, "laniakea-dock %s\n", LA_DOCK_VERSION);
            return 0;
        }
    }

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
    QObject::connect(&widget, &DockWidget::geometryChanged,
                     &dock, &Dock::widgetGeometryChanged);
    widget.setAttribute(Qt::WA_X11NetWmWindowTypeDock);
    widget.setSource(url);

    widget.show();

    return app.exec();
}
