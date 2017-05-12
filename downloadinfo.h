#ifndef DOWNLOADINFO_H
#define DOWNLOADINFO_H

#include <QString>
#include <QUrl>

struct DownloaderInfo {
    QString videoUrl, audioUrl;
    int videoSegments, audioSegments;
    QString videoInitSegment, audioInitSegment;
    QUrl fileOutputPath;
};

#endif // DOWNLOADINFO_H
