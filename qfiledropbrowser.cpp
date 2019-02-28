#include "qfiledropbrowser.h"
#include <QMimeData>
#include <QMessageBox>
#include <taglib/taglib.h>
#include <taglib/fileref.h>

QFileDropBrowser::QFileDropBrowser(QWidget *parent)
    :QTextBrowser (parent)
{
    setAcceptDrops(true);
    mLength = QTime(0, 0);
}

void QFileDropBrowser::setUrls(const QString &pause, const QString &root)
{
    mPause    = pause;
    mRoot     = root;
}

void QFileDropBrowser::resetTime()
{
    mLength = QTime(0, 0);
}

void QFileDropBrowser::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void QFileDropBrowser::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}

void QFileDropBrowser::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
}

void QFileDropBrowser::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        emit updateStrings();
        MediaInfo_t mediaInfo;

        for (const auto& a : event->mimeData()->urls())
        {
            if (mediaFileInfo(a, mediaInfo) == 0)
            {
                mLength = mLength.addSecs(mediaInfo.value("length").toInt());
                emit updateLength(mLength.toString("h:mm:ss"));
                emit updateTitle(mediaInfo.value("title").toString());
                append(fixFileUrl(a));

                if (!mPause.isEmpty() && (mediaFileInfo(mPause, mediaInfo) == 0))
                {
                    mLength = mLength.addSecs(mediaInfo.value("length").toInt());
                    emit updateLength(mLength.toString("h:mm:ss"));
                    append(fixFileUrl(mPause));
                }
            }
        }
    }
}

QString QFileDropBrowser::fixFileUrl(const QUrl &in)
{
    QString ret = QUrl::fromPercentEncoding(in.toString().toUtf8());

    if (ret.indexOf(mRoot) == 0)
    {
        ret.replace(mRoot, QString(FIIO_ROOT_PATH));
        ret.replace('/', '\\');
    }

    return ret;
}

int QFileDropBrowser::mediaFileInfo(const QUrl &in, QFileDropBrowser::MediaInfo_t &mediaInfo)
{
    int ret = 0;
    QString         tmp = QUrl::fromPercentEncoding(in.toString(QUrl::RemoveScheme).toUtf8());
    TagLib::FileRef f(TagLib::FileName(tmp.toUtf8().constData()));

    if (f.isNull())
    {
        ret = -1;
    }
    else
    {
        mediaInfo.clear();
        mediaInfo["url"   ] = in;
        mediaInfo["length"] = f.audioProperties()->length();
        mediaInfo["title" ] = QString::fromUtf8(f.tag()->title().toCString(true));
        mediaInfo["artist"] = QString::fromUtf8(f.tag()->artist().toCString(true));
        mediaInfo["album" ] = QString::fromUtf8(f.tag()->album().toCString(true));
        mediaInfo["year"  ] = f.tag()->year();
    }

    return ret;
}
