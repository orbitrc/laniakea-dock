#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QQuickWidget>

#include "Dock.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    Dock dock;
    engine.rootContext()->setContextProperty("Dock", QVariant::fromValue(&dock));

    QQuickWidget widget(&engine, nullptr);
    widget.setAttribute(Qt::WA_X11NetWmWindowTypeDock);
    widget.setSource(url);

    widget.show();

    return app.exec();
}
