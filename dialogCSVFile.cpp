/*
 * Copyright 2016-2019 Green Radio Software Solutions (GRSS)
 * All rights reserved
 * */
#include "dialogCSVFile.h"
#include "ui_dialogCSVFile.h"

DialogCSVFile::DialogCSVFile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCSVFile)
{
    ui->setupUi(this);
}//DialogAddressBook

DialogCSVFile::~DialogCSVFile()
{
    delete ui;
}

bool DialogCSVFile::initialize(QString CSVFileName, QString columnName, QLineEdit *labelToSet)
{
    foreach (QObject *object, myObjectsList) {
        if(dynamic_cast<const ClassLog *>(object) != nullptr){
            log = qobject_cast<ClassLog *>(object);
        }
        if(dynamic_cast<const ClassSettings *>(object) != nullptr){
            settings = qobject_cast<ClassSettings *>(object);

        }
    }
    //            //log->write(Q_FUNC_INFO,true);
    //    this->CSVFile.setFileName(QApplication::applicationDirPath()+"/../shared_data/"+CSVFileName);
    this->CSVFile.setFileName(CSVFileName);
    this->CSVFileInitialized = this->readCSVFile();
    log->write("CSV file exists: " + QString::number((int)this->CSVFile.exists()),false);
    this->columnToRetrieve = columnName;
    this->textEditToSet = labelToSet;
    if(this->columnToRetrieve.isEmpty() || this->textEditToSet == nullptr){
        ui->pushButtonAdd->setEnabled(false);
    }
    this->setWindowTitle(CSVFileName);
    return (log != nullptr && settings != nullptr && this->CSVFileInitialized);
}//initialize

bool DialogCSVFile::getCSVFileInitialized() const
{
    return CSVFileInitialized;
}//getAddressBookInitialized

bool DialogCSVFile::readCSVFile()
{
    QString fileName = this->CSVFile.fileName();
    QFile file(fileName);
    if(!file.exists()){
        fileName.prepend(QApplication::applicationDirPath()+"/../shared_data/");
        file.setFileName(fileName);
        if(!file.exists()){
            fileName = this->CSVFile.fileName();
            fileName.prepend(QApplication::applicationDirPath()+"/local_data/");
            file.setFileName(fileName);
            if(!file.exists()){
                log->write("ERROR: Opening CSV file!" + file.errorString(),false);
                //            ui->labelNumberRecords->setText("Records: "+QString::number(modelCSVFileData->rowCount()));
                return false;
            }
        }
    }
    this->CSVFile.setFileName(fileName);
    modelCSVFileData = new QStandardItemModel(this);
    if (file.open(QIODevice::ReadOnly)) {

        int lineindex = 0;                     // file line counter
        QTextStream in(&file);                 // read to text stream

        while (!in.atEnd()) {

            // read one line from textstream(separated by "\n")
            QString fileLine = in.readLine();

            // parse the read line into separate pieces(tokens) with "," as the delimiter
            QStringList lineToken = fileLine.split(",");

            if(lineindex == 0){
                modelCSVFileData->setHorizontalHeaderLabels(lineToken);
                lineindex++;
                continue;
            }

            // load parsed data to model accordingly
            for (int j = 0; j < lineToken.size(); j++) {
                QString value = lineToken.at(j);
                QStandardItem *item = new QStandardItem(value);
                modelCSVFileData->setItem(lineindex-1, j, item);
            }

            lineindex++;
        }

        file.close();
        ui->tableView->setModel(modelCSVFileData);

        headerData.clear();
        for (int i = 0; i < modelCSVFileData->columnCount(); ++i) {
            headerData.append(modelCSVFileData->headerData(i,Qt::Horizontal).toString());
        }
        ui->labelNumberRecords->setText("Records: "+QString::number(modelCSVFileData->rowCount()));
        return true;
    }
    else {
        log->write("ERROR: Opening CSV file!" + file.errorString(),false);
        ui->labelNumberRecords->setText("Records: "+QString::number(modelCSVFileData->rowCount()));
        return false;
    }
}//readAddressBookFile

bool DialogCSVFile::readCSVFile(QString searchString)
{
    if (this->CSVFile.open(QIODevice::ReadOnly)) {

        int lineindex = 0;                     // file line counter
        QTextStream in(&this->CSVFile);                 // read to text stream

        modelCSVFileData->clear();

        while (!in.atEnd()) {

            // read one line from textstream(separated by "\n")
            QString fileLine = in.readLine();

            // parse the read line into separate pieces(tokens) with "," as the delimiter
            QStringList lineToken = fileLine.split(",");

            if(lineindex == 0){
                modelCSVFileData->setHorizontalHeaderLabels(lineToken);
                lineindex++;
                continue;
            }

            if(!fileLine.contains(searchString, Qt::CaseInsensitive)){
                continue;
            }

            // load parsed data to model accordingly
            for (int j = 0; j < lineToken.size(); j++) {
                QString value = lineToken.at(j);
                QStandardItem *item = new QStandardItem(value);
                modelCSVFileData->setItem(lineindex-1, j, item);
            }

            lineindex++;
        }

        this->CSVFile.close();
        ui->tableView->setModel(modelCSVFileData);
        ui->labelNumberRecords->setText("Records: "+QString::number(modelCSVFileData->rowCount()));
        return true;
    }
    else {
        log->write("ERROR: Opening address book file!" + this->CSVFile.errorString(), false);
        ui->labelNumberRecords->setText("Records: "+QString::number(modelCSVFileData->rowCount()));
        return false;
    }
}//readAddressBookFile

void DialogCSVFile::on_lineEditSearch_textChanged(const QString &arg1)
{
    readCSVFile(arg1);
}//on_lineEditSearch_textChanged

void DialogCSVFile::on_pushButtonAdd_clicked()
{
    /*
  * QModelIndex index=myTableView->selectionModel()->currentIndex();
  * to get the index, then
  * QVariant value=index.sibling(index.row(),index.column()).data();
  * will get the value of the clicked cell.
  * */
    indexData.clear();
    QModelIndex index = ui->tableView->selectionModel()->currentIndex();
    for (int column = 0; column < ui->tableView->model()->columnCount(); ++column) {
        indexData.append(index.sibling(index.row(), column).data().toString());
    }
    int column = headerData.indexOf(columnToRetrieve);
    textEditToSet->setText(indexData.at(column));
    this->close();
}//on_pushButtonAdd_clicked

void DialogCSVFile::on_tableView_customContextMenuRequested(const QPoint &pos)
{
    const QString stringText = ui->tableView->model()->data(ui->tableView->indexAt(pos)).toString();
    if(!stringText.isEmpty()){
        QMenu menu;
        QAction *copyAction = menu.addAction("Copy Selected Text");
        QAction *action = menu.exec(QCursor::pos());
        if (!action)
            return;
        else if (action == copyAction) {
            QClipboard *clipboard = QGuiApplication::clipboard();
            clipboard->setText(stringText);
            this->close();
        }//showTrashAction
    }
}//on_tableView_customContextMenuRequested
