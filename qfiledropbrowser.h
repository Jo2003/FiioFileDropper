#ifndef QFILEDROPBROWSER_H
#define QFILEDROPBROWSER_H

#include <QTextBrowser>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QUrl>
#include <QTime>
#include <QMap>
#include <QVariant>

class QFileDropBrowser : public QTextBrowser
{
    Q_OBJECT

public:
    using MediaInfo_t = QMap<QString, QVariant>;
    QFileDropBrowser(QWidget *parent = nullptr);
    void setUrls(const QString& pause, const QString& root);
    void resetTime();

protected:
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent* event) override;

    QString fixFileUrl(const QUrl& in);
    int mediaFileInfo(const QUrl& in, MediaInfo_t& mediaInfo);

signals:
    void updateStrings();
    void updateLength(const QString& s);
    void updateTitle(const QString& s);

private:
    QString mRoot;
    QString mPause;
    static constexpr const char* FIIO_ROOT_PATH = "TF1:";
    QTime   mLength;
};

#endif // QFILEDROPBROWSER_H
