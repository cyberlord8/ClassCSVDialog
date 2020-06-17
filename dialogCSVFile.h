/*
 * Copyright 2016-2019 Green Radio Software Solutions (GRSS)
 * All rights reserved
 * */
#ifndef DIALOGCSVFILE_H
#define DIALOGCSVFILE_H

#include "globals.h"
#include "classsettings.h"

#include <QStandardItemModel>
namespace Ui {
class DialogCSVFile;
}

class DialogCSVFile : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCSVFile(QWidget *parent = nullptr);
    ~DialogCSVFile();
    bool initialize(QString CSVFileName, QString columnToRetrieve, QLineEdit *textEditToSet);
    bool getCSVFileInitialized() const;

signals:

private slots:
    void on_lineEditSearch_textChanged(const QString &arg1);
    void on_pushButtonAdd_clicked();
    void on_tableView_customContextMenuRequested(const QPoint &pos);

private:
    Ui::DialogCSVFile *ui;

    ClassLog *log;
    ClassSettings *settings;
    QFile CSVFile;
    bool CSVFileInitialized = false;
    QString columnToRetrieve;
    QLineEdit * textEditToSet;

    QStandardItemModel *modelCSVFileData;

    QStringList headerData, indexData;

    bool readCSVFile();
    bool readCSVFile(QString searchString);
};

#endif // DIALOGCSVFILE_H
