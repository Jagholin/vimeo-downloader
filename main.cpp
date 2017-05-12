#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "downloadinitiator.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    DownloadInitiator* myInitiator = new DownloadInitiator;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("downloadInitiator", myInitiator);
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    return app.exec();
}
