#ifndef FILEDROPPER_H
#define FILEDROPPER_H

#include <QDialog>
#include <QSettings>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QTime>

#include "qjsonmodel.h"

namespace Ui {
class FileDropper;
}

class FileDropper : public QDialog
{
    Q_OBJECT

public:
    using MediaInfo_t = QJsonObject;
    explicit FileDropper(QWidget *parent = nullptr);
    virtual ~FileDropper();

protected:
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    QString fixFileUrl(const QUrl &in);
    int mediaFileInfo(const QUrl &in, MediaInfo_t &mediaInfo);

private slots:
    void on_pushRoot_clicked();
    void on_pushPause_clicked();
    void on_pushSave_clicked();
    void on_pushClear_clicked();

private:
    Ui::FileDropper *ui;
    QJsonObject mPlayList;
    static constexpr const char* FIIO_ROOT_PATH = "TF1:";
    QTime mLength;
    QJsonModel mModel;
};

#endif // FILEDROPPER_H
