#include <QDebug>
#include "downloadinitiator.h"
#include "downloadinfo.h"
#include "downloaderthread.h"

DownloadInitiator::DownloadInitiator(QObject *parent) : QObject(parent)
{
    m_workerThread = new DownloaderThread(this);
    connect(m_workerThread, SIGNAL(stdOutput(QString)), this, SIGNAL(stdOutput(QString)));
    connect(m_workerThread, SIGNAL(progressReport(QString)), this, SIGNAL(progressReport(QString)));
}

void DownloadInitiator::initiateDownload(QString videoUrl, int videoSegments, QString videoInitSegment, QString audioUrl, int audioSegments, QString audioInitSegment, QUrl filePath)
{
    qDebug() << "Video download initiated with url: \"" << videoUrl << "\"";
    qDebug() << "segments " << videoSegments;
    qDebug() << "initSegment " << videoInitSegment;
    qDebug() << "audio url: \"" << audioUrl << "\"";
    qDebug() << "segments " << audioSegments;
    qDebug() << "initSegment " << audioInitSegment;

    DownloaderInfo myInfo;
    myInfo.videoUrl = videoUrl; myInfo.videoSegments = videoSegments; myInfo.videoInitSegment = videoInitSegment;
    myInfo.audioUrl = audioUrl; myInfo.audioSegments = audioSegments; myInfo.audioInitSegment = audioInitSegment;
    myInfo.fileOutputPath = filePath;

    qDebug() << "in downloaderInitiator, thread id " << QThread::currentThreadId();
    m_workerThread->setInfo(myInfo);
    m_workerThread->start();
    qDebug() << "in downloaderInitiator, thread started";
}
