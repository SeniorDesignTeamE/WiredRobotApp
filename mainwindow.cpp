#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>
#include <iostream>
#include <QtDebug>
#include "QMessageBox"
#include <QDialog>
#include<QFileDialog>
#define stepSize 0.000475
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    updateComboBox();
    ui->glWidget->setModel(&myModel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateComboBox()
{
    QStringList stringList;
    int position = myModel.getSelectedIndex();
    ui->comboBox->clear();
    int itemNum = myModel.getDataAmount();
    for(int i=0;i<itemNum;i++)
    {
        QString s = QString::number(i);
        QString newItem(s);
        stringList.append(s);
    }
    ui->comboBox->addItems(stringList);
    ui->comboBox->setCurrentIndex(position);
}

void MainWindow::on_insertNextBtn_clicked()
{
    coordinate* formData = getCoordinateField();
    if(formData)
    {
        if(!myModel.insertNewPoint(
                    formData->x,
                    formData->y,
                    formData->z,
                    formData->yaw,
                    formData->pitch,
                    formData->roll,
                    formData->time))
        {
            QMessageBox messageBox;
            messageBox.critical(0,"Error","New coordinate is out of bounds!");
            messageBox.setFixedSize(500,200);
        }

        updateComboBox();
        delete formData;
    }else
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Missing or Incorrect Data!");
        messageBox.setFixedSize(500,200);
    }
}

void MainWindow::on_pushToEndBtn_clicked()
{
    coordinate* formData = getCoordinateField();
    if(formData)
    {
        if(!myModel.pushNewPoint(
                    formData->x,
                    formData->y,
                    formData->z,
                    formData->yaw,
                    formData->pitch,
                    formData->roll,
                    formData->time))
        {
            QMessageBox messageBox;
            messageBox.critical(0,"Error","New coordinate is out of bounds!");
            messageBox.setFixedSize(500,200);
        }
        updateComboBox();
        delete formData;
    }else
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Missing or Incorrect Data!");
        messageBox.setFixedSize(500,200);
    }
}


// Changes the UI to have the correct data at the bottom, and on the image at the right
void MainWindow::updateBottomData()
{
    // Update bottom data
    coordinate selectedCoordinate = myModel.getSelectedCoordinate();
    ui->currentX->setText("X: " + QString::number(selectedCoordinate.x));
    ui->currentY->setText("Y: " + QString::number(selectedCoordinate.y));
    ui->currentZ->setText("Z: " + QString::number(selectedCoordinate.z));
    ui->currentYaw->setText("Yaw: " + QString::number(selectedCoordinate.yaw));
    ui->currentPitch->setText("Pitch: " + QString::number(selectedCoordinate.pitch));
    ui->currentRoll->setText("Roll: " + QString::number(selectedCoordinate.roll));
    ui->currentTime->setText("Time: " + QString::number(selectedCoordinate.time));
}

// Called on all changes to combo box. Make sure when the index is changed it is reflected in all the data
void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    if(index==-1)return;
    myModel.setCurrentPoint(index);
    updateBottomData();
}

void MainWindow::on_deletePointBtn_clicked()
{
    if(!myModel.deleteCurrentIdex())
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Cannot Delete Starting Point!");
        messageBox.setFixedSize(500,200);
    }
    updateComboBox();
}

coordinate* MainWindow::getCoordinateField()
{
    coordinate* newPoint = new coordinate();
    bool xOk(false), yOk(false), zOk(false), yawOk(false), pitchOk(false), rollOk(false), timeOk(false);
    newPoint->x = ui->xLineEdit->text().toDouble(&xOk);
    newPoint->y = ui->yLineEdit->text().toDouble(&yOk);
    newPoint->z = ui->zLineEdit->text().toDouble(&zOk);
    newPoint->yaw = ui->yawLineEdit->text().toDouble(&yawOk);
    newPoint->pitch = ui->pitchLineEdit->text().toDouble(&pitchOk);
    newPoint->roll = ui->rollLineEdit->text().toDouble(&rollOk);
    newPoint->time = ui->timeSecondsLineEdit->text().toDouble(&timeOk);
    if(!(xOk && yOk && zOk && yawOk && pitchOk && rollOk && timeOk))
    {
        delete newPoint;
        return NULL;
    }
    return newPoint;
}


void MainWindow::on_actionNew_triggered()
{
    //clear working data
    myModel.emptyWorkingPoints();
    updateBottomData();
    updateComboBox();

}
void MainWindow::on_actionOpen_triggered()
{
    int temp=0,count,len;
    float a, b,c,yaw,pitch,roll,t=0;
    //get file directory/name
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Trajectory"), "/home", tr("XML Files (*.XML)"));
    myModel.emptyWorkingPoints();
    QFile inputFile(fileName);
    if (inputFile.open(QIODevice::ReadOnly | QIODevice::Text)){ //get number of line in file, store in temp
       QTextStream in(&inputFile);
       while (!in.atEnd()){
          temp++;
          QString line = in.readLine();
       }
       inputFile.close();
    }
    if (inputFile.open(QIODevice::ReadOnly | QIODevice::Text)){
    QTextStream in(&inputFile);
    for(count=0; count<temp-4; count++){
       QString line = in.readLine();
       if(count>5){
           if((count-7)%5==4){ //line that holds time ex: <point time= "3.0">
               line.remove(0,17); //remove tabs and brackets
               len=line.size();
               line.remove(len-2,len-1);//remove tailing brackets
               t=line.toDouble();//save time value
           }
           else if((count-7)%5==0){//line that holds <q> values
               line.remove(0,8); // remove tabs and brackets
               QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
               len=list[5].size(); //remove tailing brackets fom time value
               list[5].remove(len-4,len-1);
               a=1000*list[0].toDouble();// convert meters to mm
               b=1000*list[1].toDouble();
               c=1000*list[2].toDouble();
               yaw=list[3].toDouble();
               pitch=list[4].toDouble();
               roll=list[5].toDouble();
               myModel.pushNewPoint(a,b,c,yaw,pitch,roll, t);
           }
       }
    }
    myModel.setCurrentPoint(0);
    inputFile.close();
    //update GUI with values
    updateBottomData();
    updateComboBox();
    return;
}



}

void MainWindow::on_actionSave_triggered()
{
    //empty function for now
}


void MainWindow::on_actionSave_As_triggered()
{
    //get file dir/name
    QString fileName = QFileDialog::getSaveFileName(this,
                tr("Save Trajectory"), "/home/untitled.txt", tr("XML Files (*.XML);; Text files (*.txt)"));
    myModel.writeToFile(fileName);
    return;

}

void MainWindow::on_playBtn_clicked()
{
    myModel.setPlay();
}

void MainWindow::on_stopBtn_clicked()
{
    myModel.setStop();
}

void MainWindow::on_actionExport_to_Teensy_triggered()
{
    std::vector <std::vector <int>> cableMatrix;
    std::ifstream infile("C:\\Users\\Martin\\Desktop\\array.txt");
    std::string line;
    std::string delimiter= ",";
    std::string subvalue;
    size_t pos = 0;
    std::vector<int> temp;
    int step, prevStep=0, totalLen;
    while(getline(infile, line)){
        while ((pos = line.find(delimiter)) != std::string::npos){
            subvalue=line.substr(0, line.find(delimiter));
            step=atof(subvalue.c_str())/stepSize;
            step=step-prevStep;
            prevStep=atof(subvalue.c_str())/stepSize;
            temp.push_back(step);
            line.erase(0, pos + delimiter.length());
        }
        temp.erase(temp.begin()); //remove first element since it is garbage
        qDebug()<<temp<<endl;
        cableMatrix.push_back(temp);
        temp.clear();
    }


    totalLen=cableMatrix.at(0).size();
    QByteArray packet;
    /* Tried pulling out datastream object but cant reset the pointer after clearing packet, had issues where packet grew to size 17*n
     *
     * QDataStream out(&packet, QIODevice::WriteOnly | QIODevice::Append);
     * out.setByteOrder(QDataStream::BigEndian);
     */
    quint8 header=170,len,operation,motor,checksum;
    quint32 accel=100000, velo=100000, steps;
    for(int count=0; count<totalLen; count++){
        //create and send 8 motor packets
        for(int motorC=0; motorC<8; motorC++){
            QDataStream out(&packet, QIODevice::WriteOnly | QIODevice::Append);
            out.setByteOrder(QDataStream::BigEndian);
            len=17;
            operation=10;
            motor=motorC;
            steps=cableMatrix.at(motorC).at(count);
            checksum=header^len^operation^motor^accel^velo^steps;
            out << header<<len<<operation<<motor<<accel<<velo<<steps<<checksum;
            //send packet now
            qDebug()<<packet.toHex()<<endl; //replace with serical comm
            packet.clear();
        }
        //create and send execute packet
        QDataStream out(&packet, QIODevice::WriteOnly | QIODevice::Append);
        out.setByteOrder(QDataStream::BigEndian);
        quint8 header=170, len=4, operation=12, checksum;
        checksum=header^len^operation;
        out<<header<<len<<operation<<checksum;
        qDebug()<<packet.toHex()<<endl;//replace with serial comm
        packet.clear();
        out.resetStatus();
    }
    return;
}


