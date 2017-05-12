#include "downloaderworker.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QThread>
#include <QProcess>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QStandardPaths>

DownloaderWorker::DownloaderWorker(QObject *parent) : QObject(parent)
{
    m_netManager = new QNetworkAccessManager(this);
    connect(m_netManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onRequestFinished(QNetworkReply*)));
    connect(m_netManager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(onSslError(QNetworkReply*,QList<QSslError>)));
    m_tempFile = new QFile(this);
    m_ffmpegProcess = new QProcess(this);

    connect(m_ffmpegProcess, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(onffmpegError(QProcess::ProcessError)));
    connect(m_ffmpegProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onffmpegFinished(int,QProcess::ExitStatus)));
    connect(m_ffmpegProcess, SIGNAL(readyReadStandardError()), this, SLOT(onffmpegStderr()));
    connect(m_ffmpegProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(onffmpegStdout()));
}

void DownloaderWorker::setInfo(const DownloaderInfo &info)
{
    m_info = info;
}

void DownloaderWorker::start()
{
    qDebug() << "In downloaderWorker, threadId " << QThread::currentThreadId();

    QString videoTempFileName = m_info.fileOutputPath.toString(QUrl::RemoveScheme | QUrl::RemoveAuthority | QUrl::RemoveFilename).mid(3) + "~videotemp.mp4";

    m_tempFile->setFileName(videoTempFileName);
    if (!m_tempFile->open(QIODevice::WriteOnly)) {
        stdOutput(tr("Can't open file to write, path %1\n").arg(videoTempFileName));
        qDebug() << "Can't open file to write, path " << videoTempFileName;
        onFinished();
        return;
    }

    QByteArray initSegment = QByteArray::fromBase64(m_info.videoInitSegment.toUtf8());
    qint64 bytesWritten = m_tempFile->write(initSegment);
    if (bytesWritten == -1) {
        stdOutput(tr("Error writing initial segment, return code -1\n"));
        qDebug() << "Error writing initial segment, return code -1\n";
        onFinished();
        return;
    }
    if (bytesWritten < initSegment.size()) {
        stdOutput(tr("Init segment wasnt written, size %1 Bytes written %2\n").arg(initSegment.size()).arg(bytesWritten));
        qDebug() << "Init segment wasnt written, size " << initSegment.size() << " Bytes written " << bytesWritten;
        onFinished();
        return;
    }

    m_currentSegment = 1;
    m_currentPhase = 0;
    downloadVideoSegment(m_currentSegment);
}

void DownloaderWorker::downloadPhase1()
{
    QString audioTempFileName = m_info.fileOutputPath.toString(QUrl::RemoveScheme | QUrl::RemoveAuthority | QUrl::RemoveFilename).mid(3) + "~audiotemp.mp3";

    m_tempFile->setFileName(audioTempFileName);
    if (!m_tempFile->open(QIODevice::WriteOnly)) {
        stdOutput(tr("Can't open file to write, path %1\n").arg(audioTempFileName));
        qDebug() << "Can't open file to write, path " << audioTempFileName;
        onFinished();
        return;
    }

    QByteArray initSegment = QByteArray::fromBase64(m_info.audioInitSegment.toUtf8());
    qint64 bytesWritten = m_tempFile->write(initSegment);
    if (bytesWritten == -1) {
        stdOutput(tr("Error writing initial segment, return code -1\n"));
        qDebug() << "Error writing initial segment, return code -1";
        onFinished();
        return;
    }
    if (bytesWritten < initSegment.size()) {
        stdOutput(tr("Init segment wasnt written, size %1 Bytes written %2\n").arg(initSegment.size()).arg(bytesWritten));
        qDebug() << "Init segment wasnt written, size " << initSegment.size() << " Bytes written " << bytesWritten;
        onFinished();
        return;
    }

    m_currentSegment = 1;
    m_currentPhase = 1;
    downloadSegment(m_currentSegment);
}

void DownloaderWorker::downloadPhase2()
{
    QString fileOutPath = m_info.fileOutputPath.toString(QUrl::RemoveScheme | QUrl::RemoveAuthority).mid(3);

    QString fileOutName = m_info.fileOutputPath.fileName();
    QString fileOutDir = m_info.fileOutputPath.toString(QUrl::RemoveScheme | QUrl::RemoveAuthority | QUrl::RemoveFilename).mid(3);
    m_ffmpegProcess->setArguments({"-nostdin", "-y", "-i", "~audiotemp.mp3", "-i", "~videotemp.mp4", "-acodec", "copy", "-vcodec", "h264", fileOutName});
    m_ffmpegProcess->setWorkingDirectory(fileOutDir);
    m_ffmpegProcess->setProgram("ffmpeg.exe");
    qDebug() << "ffmpeg arguments: " << m_ffmpegProcess->arguments().join(" ");
    m_ffmpegProcess->start();
}

void DownloaderWorker::onffmpegError(QProcess::ProcessError err)
{
    switch(err) {
    case QProcess::FailedToStart:
        stdOutput(tr("ffmpeg failed to start\n"));
        qDebug() << "ffmpeg failed to start";
        break;
    case QProcess::Crashed:
        stdOutput(tr("ffmpeg crashed\n"));
        qDebug() << "ffmpeg crashed";
        break;
    case QProcess::ReadError:
        stdOutput(tr("ffmpeg can't read\n"));
        qDebug() << "ffmpeg can't read";
        break;
    default:
        stdOutput(tr("Unknown ffmpeg error. abort\n"));
        break;
    }
    onFinished();
}

void DownloaderWorker::onffmpegFinished(int code, QProcess::ExitStatus stat)
{
    if (stat != QProcess::NormalExit) {
        stdOutput(tr("ffmpeg finished unexpectedly, error code %1\n").arg(code));
        qDebug() << "ffmpeg finished unexpectedly, error code " << code;
        onFinished();
        return;
    }

    qDebug() << "ffmpeg finished work";

    // deleting temporary files
    QString videoTempFileName = m_info.fileOutputPath.toString(QUrl::RemoveScheme | QUrl::RemoveAuthority | QUrl::RemoveFilename).mid(3) + "~videotemp.mp4";

    m_tempFile->setFileName(videoTempFileName);
    m_tempFile->remove();

    QString audioTempFileName = m_info.fileOutputPath.toString(QUrl::RemoveScheme | QUrl::RemoveAuthority | QUrl::RemoveFilename).mid(3) + "~audiotemp.mp3";

    m_tempFile->setFileName(audioTempFileName);
    m_tempFile->remove();

    onFinished();
}

void DownloaderWorker::onffmpegStderr()
{
    QByteArray content = m_ffmpegProcess->readAllStandardError();
    stdOutput(QString::fromUtf8(content));
    qDebug() << content;
}

void DownloaderWorker::onffmpegStdout()
{
    QByteArray content = m_ffmpegProcess->readAllStandardOutput();
    stdOutput(QString::fromUtf8(content));
    qDebug() << content;
}

void DownloaderWorker::onSslError(QNetworkReply *r, const QList<QSslError> &errors)
{
    stdOutput(tr("Ssl errors detected\n"));
    for (QSslError err: errors) {
        stdOutput(tr("%1\n").arg(err.errorString()));
    }
    r->ignoreSslErrors();
}

void DownloaderWorker::onRequestFinished(QNetworkReply * myReply)
{
    qDebug() << "Segment finished download, status code: " << myReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    qDebug() << "Url was " << myReply->url().toString();

    if (myReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 200) {
        stdOutput(tr("Error getting a segment, status code %1\n address: %2\n").arg(myReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()).arg(myReply->url().toString()));
        qDebug() << "Some error getting segments";
        onFinished();
        return;
    }

    QByteArray segmentData = myReply->readAll();
    qint64 bytesWritten = m_tempFile->write(segmentData);
    if (bytesWritten == -1) {
        stdOutput(tr("Cannot write segment %1, return code -1\n").arg(m_currentSegment));
        qDebug() << "Error writing segment " << m_currentSegment << ", return code -1";
        onFinished();
        return;
    }
    if (bytesWritten < segmentData.size()) {
        stdOutput(tr("Segment %1 wasn't written, size %2, bytes written %3\n").arg(m_currentSegment).arg(segmentData.size()).arg(bytesWritten));
        qDebug() << "Segment " << m_currentSegment << " wasnt written, size " << segmentData.size() << " Bytes written " << bytesWritten;
        onFinished();
        return;
    }

    QString downloadType(m_currentPhase == 0 ? "video" : "audio");
    progressReport(tr("Downloaded %1 segment %2 out of %3").arg(downloadType).arg(m_currentSegment).arg(m_currentPhase == 0 ? m_info.videoSegments : m_info.audioSegments));

    if ((m_currentSegment == m_info.videoSegments && m_currentPhase == 0) ||
            (m_currentSegment == m_info.audioSegments && m_currentPhase == 1)) {
        m_tempFile->flush();
        m_tempFile->close();
        qDebug("File written successfully");

        if (m_currentPhase == 0)
            downloadPhase1();
        else if (m_currentPhase == 1)
            downloadPhase2();
        else
            onFinished();
        return;
    }

    downloadSegment(++m_currentSegment);
}

void DownloaderWorker::downloadVideoSegment(int segNumber)
{
    QUrl videoUrl = QUrl(m_info.videoUrl + "segment-" + QString::number(segNumber) + ".m4s");
    videoUrl = videoUrl.adjusted(QUrl::NormalizePathSegments);
    QNetworkRequest myRequest(videoUrl);
    m_netManager->get(myRequest);
}

void DownloaderWorker::downloadAudioSegment(int segNumber)
{
    QUrl audioUrl = QUrl(m_info.audioUrl + "segment-" + QString::number(segNumber) + ".m4s");
    audioUrl = audioUrl.adjusted(QUrl::NormalizePathSegments);
    QNetworkRequest myRequest(audioUrl);
    m_netManager->get(myRequest);
}

void DownloaderWorker::downloadSegment(int segNumber)
{
    if (m_currentPhase == 0)
        downloadVideoSegment(segNumber);
    else if (m_currentPhase == 1)
        downloadAudioSegment(segNumber);
}

void DownloaderWorker::saveDownloadState()
{
    QJsonObject rootObject{};
    QJsonObject infoObject{};

    auto infoObjectData[] = {
        {"videoUrl", QJsonValue{m_info.videoUrl} },
        {"videoSegments", QJsonValue{m_info.videoSegments} },
        {"videoInitSegment", QJsonValue{m_info.videoInitSegment} },
        {"audioUrl", QJsonValue{m_info.audioUrl} },
        {"audioSegments", QJsonValue{m_info.audioSegments} },
        {"audioInitSegment", QJsonValue{m_info.audioInitSegment} },
        {"fileSavePath", QJsonValue{m_info.fileOutputPath} },
    };

    for (auto info: infoObjectData) {
        infoObject.insert(info[0], info[1]);
    }

    rootObject.insert("videoInfo", QJsonValue{infoObject});
    rootObject.insert("currentPhase", QJsonValue{m_currentPhase});
    rootObject.insert("currentSegment", QJsonValue{m_currentSegment});

    QJsonDocument saveData{};
    saveData.setObject(rootObject);
    saveData.toJson();

    QFile
}
