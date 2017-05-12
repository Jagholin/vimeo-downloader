#ifndef DOWNLOADERWORKER_H
#define DOWNLOADERWORKER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QFile>
#include <QProcess>
#include "downloadinfo.h"

class DownloaderWorker : public QObject
{
    Q_OBJECT
public:
    explicit DownloaderWorker(QObject *parent = 0);

    void setInfo(DownloaderInfo const&);

signals:
    void onFinished();
    void stdOutput(QString);
    void progressReport(QString);

public slots:
    void start();
    void onRequestFinished(QNetworkReply*);

    void onffmpegError(QProcess::ProcessError);
    void onffmpegFinished(int code, QProcess::ExitStatus);

    void onffmpegStderr();
    void onffmpegStdout();

    void onSslError(QNetworkReply*, const QList<QSslError> &errors);

protected:
    DownloaderInfo m_info;
    QNetworkAccessManager* m_netManager;
    QFile* m_tempFile;
    QProcess* m_ffmpegProcess;

    int m_currentSegment;
    int m_currentPhase;

    void downloadVideoSegment(int segNumber);
    void downloadAudioSegment(int segNumber);
    void downloadSegment(int segNumber);
    void saveDownloadState();
    void restoreDownloadState();

    void downloadPhase1();
    void downloadPhase2();
};

#endif // DOWNLOADERWORKER_H
