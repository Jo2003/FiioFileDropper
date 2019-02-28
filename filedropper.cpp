#include "filedropper.h"
#include "ui_filedropper.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

FileDropper::FileDropper(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileDropper)
{
    ui->setupUi(this);
    connect(ui->textBrowser, &QFileDropBrowser::updateStrings, this, &FileDropper::updateStrings);
    connect(ui->textBrowser, &QFileDropBrowser::updateLength, this, &FileDropper::lengthUpdated);
    connect(ui->textBrowser, &QFileDropBrowser::updateTitle, this, &FileDropper::songUpdated);

    QSettings set;
    ui->lineRoot->setText(set.value("root_folder").toString());
    ui->linePauseAudio->setText(set.value("pause_file").toString());
}

FileDropper::~FileDropper()
{
    delete ui;
}

void FileDropper::updateStrings()
{
    ui->textBrowser->setUrls(ui->linePauseAudio->text(),
                             ui->lineRoot->text());
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
            toSave.write(ui->textBrowser->toPlainText().toUtf8());
        }
    }
}

void FileDropper::lengthUpdated(const QString &s)
{
    ui->labLength->clear();
    ui->labLength->setText(s);
}

void FileDropper::songUpdated(const QString &s)
{
    ui->labSong->clear();
    ui->labSong->setText(s);
}

void FileDropper::on_pushClear_clicked()
{
    ui->textBrowser->clear();
    ui->textBrowser->resetTime();
    ui->labLength->clear();
    ui->labLength->setText("0:00:00");
    ui->labSong->clear();
}
