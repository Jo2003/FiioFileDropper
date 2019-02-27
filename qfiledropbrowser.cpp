#include "qfiledropbrowser.h"
#include <QMimeData>
#include <QMessageBox>

QFileDropBrowser::QFileDropBrowser(QWidget *parent)
    :QTextBrowser (parent)
{
    setAcceptDrops(true);
}

void QFileDropBrowser::setUrls(const QString &pause, const QString &root)
{
    mPause    = pause;
    mRoot     = root;
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

        for (const auto& a : event->mimeData()->urls())
        {
            append(fixFileUrl(a));
            if (!mPause.isEmpty())
            {
                append(fixFileUrl(mPause));
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
