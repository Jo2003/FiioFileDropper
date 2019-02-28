#include "filedropper.h"
#include "ui_filedropper.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QMimeData>
#include <taglib/taglib.h>
#include <taglib/fileref.h>

FileDropper::FileDropper(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileDropper)
{
    ui->setupUi(this);

    QSettings set;
    ui->lineRoot->setText(set.value("root_folder").toString());
    ui->linePauseAudio->setText(set.value("pause_file").toString());
    mLength = QTime(0, 0);
    ui->treeView->setModel(&mModel);
    setAcceptDrops(true);
}

FileDropper::~FileDropper()
{
    delete ui;
}

void FileDropper::on_pushRoot_clicked()
{
    QString folder = QFileDialog::getExistingDirectoryUrl(this, tr("Open root folder")).toString();
    if (!folder.isEmpty())
    {
        ui->lineRoot->setText(folder);
        QSettings set;
        set.setValue("root_folder", folder);
    }
}

void FileDropper::on_pushPause_clicked()
{
    QString pause = QFileDialog::getOpenFileUrl(this,
        tr("Open Pause Audio File"), QString(),
        tr("Audio Files (*.wav *.ogg *.mp3 *.m4a *.flac);;All Files (*.*)")).toString();

    QSettings set;
    set.setValue("pause_file", pause);
    ui->linePauseAudio->setText(pause);
}

void FileDropper::on_pushSave_clicked()
{
    QString filter;
    QString save = QFileDialog::getSaveFileName(this, tr("Save Playlist"), "", tr("Playlist (*.m3u);;All Files (*.*)"), &filter);

    if (!save.isEmpty())
    {
        if (filter.contains(tr("Playlist")) && !save.contains(".m3u"))
        {
            save += ".m3u";
        }

        QFile toSave(save);
        if (toSave.open(QIODevice::Truncate|QIODevice::WriteOnly))
        {
            for (const auto& a : mPlayList.keys())
            {
                const auto& o = mPlayList.value(a).toObject();
                toSave.write(o.value("url").toString().toUtf8() + "\n");

                if (!ui->linePauseAudio->text().isEmpty())
                {
                    toSave.write(fixFileUrl(QUrl(ui->linePauseAudio->text())).toUtf8() + "\n");
                }
            }
        }
    }
}

void FileDropper::on_pushClear_clicked()
{
    ui->labLength->clear();
    ui->labLength->setText("0:00:00");
    mPlayList = QJsonObject();
    mModel.loadJson(QByteArray("{}"));
}

void FileDropper::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void FileDropper::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}

void FileDropper::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
}

void FileDropper::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        MediaInfo_t mediaInfo;

        for (const auto& a : event->mimeData()->urls())
        {
            if (mediaFileInfo(a, mediaInfo) == 0)
            {
                mLength = mLength.addSecs(mediaInfo.value("length").toInt());
                ui->labLength->clear();
                ui->labLength->setText(mLength.toString("h:mm:ss"));

                mPlayList.insert(QString("%1. %2").arg(mPlayList.count() + 1, 3, 10, QChar('0'))
                                 .arg(mediaInfo.value("title").toString()), mediaInfo);

                QJsonDocument jdock(mPlayList);

                mModel.loadJson(jdock.toJson());
                ui->treeView->expandAll();
                ui->treeView->scrollToBottom();

                if (!ui->linePauseAudio->text().isEmpty() && (mediaFileInfo(ui->linePauseAudio->text(), mediaInfo) == 0))
                {
                    mLength = mLength.addSecs(mediaInfo.value("length").toInt());
                    ui->labLength->clear();
                    ui->labLength->setText(mLength.toString("h:mm:ss"));
                }
            }
        }
    }
}

QString FileDropper::fixFileUrl(const QUrl &in)
{
    QString ret = QUrl::fromPercentEncoding(in.toString().toUtf8());

    if (ret.indexOf(ui->lineRoot->text()) == 0)
    {
        ret.replace(ui->lineRoot->text(), QString(FIIO_ROOT_PATH));
        ret.replace('/', '\\');
    }

    return ret;
}

int FileDropper::mediaFileInfo(const QUrl &in, FileDropper::MediaInfo_t &mediaInfo)
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
        mediaInfo = QJsonObject();
        mediaInfo["url"   ] = fixFileUrl(QUrl::fromPercentEncoding(in.toString().toUtf8()));
        mediaInfo["length"] = f.audioProperties()->length();
        mediaInfo["title" ] = QString::fromUtf8(f.tag()->title().toCString(true));
        mediaInfo["artist"] = QString::fromUtf8(f.tag()->artist().toCString(true));
        mediaInfo["album" ] = QString::fromUtf8(f.tag()->album().toCString(true));
        mediaInfo["year"  ] = static_cast<int>(f.tag()->year());
    }

    return ret;
}
