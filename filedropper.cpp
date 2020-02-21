#include "filedropper.h"
#include "ui_filedropper.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QMimeData>
#include <taglib/taglib.h>
#include <taglib/fileref.h>
#include <taglib/mpegfile.h>

FileDropper::FileDropper(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileDropper)
{
    ui->setupUi(this);

    QSettings set;
    ui->lineRoot->setText(set.value("root_folder").toString());
    ui->linePauseAudio->setText(set.value("pause_file").toString());
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
                ui->labLength->clear();

                mPlayList.insert(QString("%1").arg(mPlayList.count() + 1, 3, 10, QChar('0')), mediaInfo);

                QJsonDocument jdock(mPlayList);

                mModel.loadJson(jdock.toJson());
                ui->treeView->expandAll();
                ui->treeView->scrollToBottom();

                countTime();
            }
        }
    }
}

QString FileDropper::fixFileUrl(const QUrl &in) const
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

void FileDropper::reCreateTitleIndex()
{
    QJsonValue mediaInfo;
    int i = 0;
    QString title;
    for (const auto& k : mPlayList.keys())
    {
        // cache media info
        mediaInfo = mPlayList.take(k);

        title = QString("%1").arg(++i, 3, 10, QChar('0'));

        mPlayList.insert(title, mediaInfo);
    }
}

void FileDropper::countTime()
{
    MediaInfo_t mediaInfo;
    QTime length = QTime(0, 0);

    if (!ui->linePauseAudio->text().isEmpty() && (mediaFileInfo(ui->linePauseAudio->text(), mediaInfo) == 0))
    {
        length = length.addSecs((mPlayList.count() - 1) * mediaInfo.value("length").toInt());
    }

    for (const auto& t : mPlayList)
    {
        length = length.addSecs(t.toObject()["length"].toInt());
    }

    ui->labLength->clear();
    ui->labLength->setText(length.toString("h:mm:ss"));

}

void FileDropper::on_pushItemDelete_clicked()
{
    QModelIndex idx = ui->treeView->currentIndex();

    if (idx.isValid())
    {
        if (idx.parent().isValid())
        {
            idx = idx.parent();
        }
        mPlayList.remove(idx.data().toString());
        reCreateTitleIndex();

        QJsonDocument jdock(mPlayList);

        mModel.loadJson(jdock.toJson());
        ui->treeView->expandAll();
        ui->treeView->scrollToBottom();

        countTime();
    }
}

void FileDropper::on_pushItemUp_clicked()
{
    int pos = 0, i = 0;
    QModelIndex idx = ui->treeView->currentIndex();

    if (idx.isValid())
    {
        if (idx.parent().isValid())
        {
            idx = idx.parent();
        }

        pos = idx.data().toString().toInt();

        if ((mPlayList.count() > 1) && (pos != 1))
        {
            QJsonObject tmpList;
            QJsonValue  toMove = mPlayList.take(idx.data().toString());

            // new position
            pos --;

            for (const auto& k : mPlayList.keys())
            {
                i ++;

                if (i == pos)
                {
                    tmpList.insert(QString("%1").arg(i, 3, 10, QChar('0')), toMove);
                    i++;
                }

                tmpList.insert(QString("%1").arg(i, 3, 10, QChar('0')), mPlayList.value(k));
            }

            mPlayList = tmpList;

            /////////////////////////////////
            QJsonDocument jdock(mPlayList);
            mModel.loadJson(jdock.toJson());
            ui->treeView->expandAll();
            ui->treeView->scrollToBottom();
            countTime();
        }
    }
}

void FileDropper::on_pushItemDown_clicked()
{
    int pos = 0, i = 0;
    QModelIndex idx = ui->treeView->currentIndex();

    if (idx.isValid())
    {
        if (idx.parent().isValid())
        {
            idx = idx.parent();
        }

        pos = idx.data().toString().toInt();

        if ((mPlayList.count() > 1) && (pos < mPlayList.count()))
        {
            QJsonObject tmpList;
            QJsonValue  toMove = mPlayList.take(idx.data().toString());

            // new position
            pos ++;

            for (const auto& k : mPlayList.keys())
            {
                i ++;
                tmpList.insert(QString("%1").arg(i, 3, 10, QChar('0')), mPlayList.value(k));

                if ((i + 1) == pos)
                {
                    i++;
                    tmpList.insert(QString("%1").arg(i, 3, 10, QChar('0')), toMove);
                }
            }

            mPlayList = tmpList;

            /////////////////////////////////
            QJsonDocument jdock(mPlayList);
            mModel.loadJson(jdock.toJson());
            ui->treeView->expandAll();
            ui->treeView->scrollToBottom();
            countTime();
        }
    }
}
