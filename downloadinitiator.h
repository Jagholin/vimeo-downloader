#ifndef DOWNLOADINITIATOR_H
#define DOWNLOADINITIATOR_H

#include <QObject>
#include "downloaderthread.h"

class DownloadInitiator : public QObject
{
    Q_OBJECT
public:
    explicit DownloadInitiator(QObject *parent = 0);

protected:
    DownloaderThread* m_workerThread;

signals:
    void stdOutput(QString);
    void progressReport(QString);

public slots:
    void initiateDownload(QString videoUrl, int videoSegments, QString videoInitSegment,
                          QString audioUrl, int audioSegments, QString audioInitSegment, QUrl filePath);
};

#endif // DOWNLOADINITIATOR_H
