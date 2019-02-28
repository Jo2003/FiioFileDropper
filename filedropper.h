#ifndef FILEDROPPER_H
#define FILEDROPPER_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class FileDropper;
}

class FileDropper : public QDialog
{
    Q_OBJECT

public:
    explicit FileDropper(QWidget *parent = nullptr);
    ~FileDropper();

private slots:
    void updateStrings();
    void on_pushRoot_clicked();
    void on_pushPause_clicked();
    void on_pushSave_clicked();
    void lengthUpdated(const QString& s);
    void songUpdated(const QString& s);
    void on_pushClear_clicked();

private:
    Ui::FileDropper *ui;
};

#endif // FILEDROPPER_H
