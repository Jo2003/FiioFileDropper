/** \file  filedropper.h
    \brief A tool to create playlists for FiiO X1

    # Fiio File Dropper #

    Using this tool you can create playlists for and on the FiiO X1.
    To do so, connect the device to your PC, enter the path to the FiiO
    storage in "Root Folder". You may specify an audiofile which can act as pause.
    This file must also be located on your FiiO device.

    ## Why? ##
    One may ask: Why you need this crap? The answer might sound stupid as well:
    You can create a playlist with pauses which you can record as Mixtape to an
    old compact cassette. You can create a mix which will fit on your cassette
    and has pauses as needed for music search on such old devices.

    ## Licence ##
    (c)2020 by Joerg Neubert

    This is free software. Use it as you like. I'm not responsible for anything
    which might happen when using this software.

    If you can't accept this, don't use it!
*/

#pragma once

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

///
/// \brief The FileDropper class
///
class FileDropper : public QDialog
{
    Q_OBJECT

public:
    /// \brief ease namespace usage
    using MediaInfo_t = QJsonObject;

    ///
    /// \brief FileDropper (constructor)
    /// \param parent parent widget
    ///
    explicit FileDropper(QWidget *parent = nullptr);

    ///
    /// \brief ~FileDropper (destructor)
    ///
    virtual ~FileDropper();

protected:
    /// @{
    ///
    /// \brief drag / leave event stuff
    /// \param event
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    /// @}

    ///
    /// \brief fix file url
    /// \param[in] in url tofix
    /// \return fixed url as string
    ///
    QString fixFileUrl(const QUrl &in) const;

    ///
    /// \brief grab media info from file
    /// \param[in] in url to get media info from
    /// \param[out] media info from url
    /// \return 0 -> ok; else -> error
    ///
    int mediaFileInfo(const QUrl &in, MediaInfo_t &mediaInfo);

    ///
    /// \brief recreate numbering in playlist
    ///
    void reCreateTitleIndex();

    ///
    /// \brief sum up time of playlist including pauses
    ///
    void countTime();

private slots:
    /// @{
    /// \brief push button slots
    void on_pushRoot_clicked();
    void on_pushPause_clicked();
    void on_pushSave_clicked();
    void on_pushClear_clicked();
    void on_pushItemDelete_clicked();
    void on_pushItemUp_clicked();
    void on_pushItemDown_clicked();
    /// @}

private:
    /// \brief the UI
    Ui::FileDropper *ui;

    /// \brief the playlist
    QJsonObject mPlayList;

    /// \brief root path used on FiiO X1 device
    static constexpr const char* FIIO_ROOT_PATH = "TF1:";

    /// \brief model for tree view
    QJsonModel mModel;
};

