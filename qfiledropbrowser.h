#ifndef QFILEDROPBROWSER_H
#define QFILEDROPBROWSER_H

#include <QTextBrowser>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QUrl>

class QFileDropBrowser : public QTextBrowser
{
    Q_OBJECT

public:
    QFileDropBrowser(QWidget *parent = nullptr);
    void setUrls(const QString& pause, const QString& root);

protected:
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent* event) override;

    QString fixFileUrl(const QUrl& in);

signals:
    void updateStrings();

private:
    QString mRoot;
    QString mPause;
    static constexpr const char* FIIO_ROOT_PATH = "TF1:";
};

#endif // QFILEDROPBROWSER_H
