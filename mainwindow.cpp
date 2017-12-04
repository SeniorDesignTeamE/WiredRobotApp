#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>
#include <iostream>
#include <QtDebug>
#include "QMessageBox"
#include <QDialog>
#include<QFileDialog>
#include <QtSerialPort/QSerialPort>
#define stepSize 0.475
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    updateComboBox();
    ui->glWidget->setModel(&myModel);
    ui->xSlider->setValue(50);
    ui->ySlider->setValue(50);
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
    int count=0,totalLen;
    std::vector <std::vector <float>> cableMatrix2;
    std::vector <std::vector <int>> stepMat;
    std::vector<int> temp2;
    for(std::vector<float> item: myModel.getAllWireLengths())
    {
        // each of these items holds a vector that has the lengths that the wires need to be in mm
        if(count==0){
        }
        else{
            cableMatrix2.push_back(item);
            qDebug()<<item;
        }
        count++;
    }
    if(count==1){
        return;
    }
    totalLen=count-1;
    for(int i=1; i<totalLen; i++){
        for(int j=0; j<8; j++){
            int diff= ((int)(cableMatrix2[i][j]/stepSize))-((int)(cableMatrix2[i-1][j]/stepSize));
            temp2.push_back(diff);
        }
        stepMat.push_back(temp2);
        qDebug()<<temp2;
        temp2.clear();
    }

    QSerialPort port;
    QByteArray exit;
    QDataStream e(&exit, QIODevice::WriteOnly | QIODevice::Append);
    QByteArray packet;
    quint8 header=170,exitCode=180,len,operation,motor,checksum;
    qint32 accel=200, velo=20000, steps;
    e.setByteOrder(QDataStream::BigEndian);
    e<<exitCode;
    port.setPortName("COM5");
    port.setBaudRate(QSerialPort::Baud115200);
    port.setParity(QSerialPort::NoParity);
    port.setStopBits(QSerialPort::OneStop);
    port.setFlowControl(QSerialPort::NoFlowControl);

    std::vector <QByteArray> packetSet;
    std::vector<std::vector<QByteArray>> packetMatrix;
    for(int count=0; count<totalLen-1; count++){
        for(int motorC=0; motorC<8; motorC++){
            QDataStream out(&packet, QIODevice::WriteOnly | QIODevice::Append);
            out.setByteOrder(QDataStream::BigEndian);
            len=17;
            operation=11;
            motor=motorC;
            checksum=0;
            steps=stepMat.at(count).at(motorC);
            out << header<<len<<operation<<motor<<accel<<velo<<steps;
            for(int i=0; i<packet.size(); i++){
                checksum=checksum^packet[i];
            }
            out<<checksum;
            packetSet.push_back(packet);
            packet.clear();
        }
        QDataStream out(&packet, QIODevice::WriteOnly | QIODevice::Append);
        out.setByteOrder(QDataStream::BigEndian);
        len=4;
        operation=12;
        checksum=0;
        out << header<<len<<operation;
        for(int i=0; i<packet.size(); i++){
            checksum=checksum^packet[i];
        }
        out<<checksum;
        packetSet.push_back(packet);
        packetMatrix.push_back(packetSet);
        qDebug() << packetSet;
        packet.clear();
        packetSet.clear();
    }
    if(!port.open(QIODevice::ReadWrite)){
        qDebug() << port.errorString();
        qDebug()  << port.portName();
        return;
    }
    for(int count=0; count<totalLen-1; count++){
        for(int motorC=0; motorC<8; motorC++){
            port.write(packetMatrix.at(count).at(motorC).constData(), 17);
            port.flush();
            while(!port.waitForReadyRead(-1)){
                Sleep(1);
            }
            QByteArray ret=port.readAll();
            qDebug()<<ret.toHex().toUpper();
        }

        port.write(packetMatrix.at(count).at(8).constData(), 4);
        port.flush();
        while(!port.waitForReadyRead(-1)){
            Sleep(1);
        }
        QByteArray ret=port.readAll();
        qDebug()<<ret.toHex().toUpper();
    }
    port.write(exit.constData(), 1);
    port.flush();
    port.close();
    return;
}


void MainWindow::on_xSlider_valueChanged(int value)
{
    myModel.setEyeRotationX(value);
}

void MainWindow::on_ySlider_valueChanged(int value)
{
    myModel.setEyeRotationY(value);
}
