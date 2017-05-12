#ifndef DOWNLOADERTHREAD_H
#define DOWNLOADERTHREAD_H

#include <QObject>
#include <QThread>
#include "downloadinfo.h"

class DownloaderThread : public QThread
{
    Q_OBJECT
public:
    explicit DownloaderThread(QObject *parent = 0);

    void setInfo(DownloaderInfo const&);

signals:
    void stdOutput(QString);
    void progressReport(QString);

public slots:

protected:
    DownloaderInfo m_info;

    // QThread interface
protected:
    void run() override;
};

#endif // DOWNLOADERTHREAD_H
