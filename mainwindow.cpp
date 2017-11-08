#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateComboBox(Model model)
{
    QStringList stringList;
    ui->comboBox->clear();
    int itemNum = model.getDataAmount();
    for(int i=0;i<itemNum;i++)
    {
        QString s = QString::number(i);
        QString newItem(s);
        stringList.append(s);
    }
    ui->comboBox->addItems(stringList);
    int position = myModel.getCurrentPoint();
    qDebug() << position;
    ui->comboBox->setCurrentIndex(position);
}

void MainWindow::on_insertNextBtn_clicked()
{
    myModel.insertNewPoint(30,30,30,30,30,30,30);
    updateComboBox(myModel);
}

void MainWindow::on_pushToEndBtn_clicked()
{
    myModel.pushNewPoint(30,30,30,30,30,30,30);
    updateComboBox(myModel);
}
