#include "downloaderthread.h"
#include "downloaderworker.h"
#include <QDebug>

DownloaderThread::DownloaderThread(QObject *parent) : QThread(parent)
{

}

void DownloaderThread::setInfo(const DownloaderInfo &info)
{
    m_info = info;
}


void DownloaderThread::run()
{
    qDebug() << "In thread run, thread id " << QThread::currentThreadId();
    DownloaderWorker* myWorker = new DownloaderWorker;
    myWorker->moveToThread(this);
    myWorker->setInfo(m_info);

    connect(myWorker, SIGNAL(onFinished()), this, SLOT(quit()));
    connect(myWorker, SIGNAL(stdOutput(QString)), this, SIGNAL(stdOutput(QString)));
    connect(myWorker,SIGNAL(progressReport(QString)), this, SIGNAL(progressReport(QString)));
    QMetaObject::invokeMethod(myWorker, "start", Qt::QueuedConnection);

    qDebug() << "In thread run, starting event loop";
    exec();

    qDebug() << "Stop thread run, thread id " << QThread::currentThreadId();
    delete myWorker;
}
