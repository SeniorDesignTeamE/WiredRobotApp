#include "model.h"
#include <QtDebug>
#include <QFile>
#include <glm/vec3.hpp>
#define trajDir "C:\\Users\\Martin\\Downloads\\CASPR-master\\CASPR-master\\data\\model_config\\models\\SCDM\\spatial_manipulators\\PoCaBot_spatial\\PoCaBot_spatial_trajectories.xml"
#define fileHeader "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<!DOCTYPE trajectories SYSTEM \"../../../../templates/trajectories.dtd\">\n<trajectories>\n\t<joint_trajectories>\n\t\t<quintic_spline_trajectory id=\"traj_1\" time_definition = \"relative\" time_step=\"0.05\">\n\t\t\t<points>"
#define fileCloser "\n\t\t\t</points>\n\t\t</quintic_spline_trajectory>\n\t</joint_trajectories>\n</trajectories>"
#define openPoint "\n\t\t\t\t<point"
#define closePoint "\n\t\t\t\t</point>"
#define qDot "\n\t\t\t\t\t<q_dot>0.0 0.0 0.0 0.0 0.0 0.0</q_dot>\n\t\t\t\t\t<q_ddot>0.0 0.0 0.0 0.0 0.0 0.0</q_ddot>"



Model::Model() : myCoordinates(ROBOT_X_MIN,ROBOT_Y_MIN,ROBOT_Z_MIN)
{
    robotFrame = makeFrame();
}

// Push point to the end of the model
bool Model::pushNewPoint(float x, float y, float z, float yaw, float pitch, float roll, float time)
{
    if(isPointValid(x,y,z,yaw,pitch,roll))
    {
        myCoordinates.pushNewCoordinate(x,y,z,yaw,pitch,roll,time);
        currentPoint+=1;
        return true;
    }
    return false;
}

// Deletes the currently selected index
// Returns false if it can't delete that point
bool Model::deleteCurrentIdex()
{
    // Don't delete the starting point D:
    if(currentPoint==0) return false;
    myCoordinates.deleteIndex(currentPoint);
    setCurrentPoint(currentPoint-1);
    return true;
}

// Clear all points from the coordinate list except the first one
void Model::emptyWorkingPoints()
{
    myCoordinates.clearAllPoints();
}

// Inserts new point after the current position
// Returns false if the point couldn't be added
bool Model::insertNewPoint(float x, float y, float z, float yaw, float pitch, float roll,float time)
{
    if(isPointValid(x,y,z,yaw,pitch,roll))
    {
        myCoordinates.insertNewCoordinate(x,y,z,yaw,pitch,roll,time,currentPoint+1);
        setCurrentPoint(currentPoint+1);
        return true;
    }
    return false;
}


// Returns if the point being added is within the bounds of the robot
bool Model::isPointValid(float x, float y, float z,float yaw,float pitch,float roll)
{
    if(x>ROBOT_X_MAX || x<ROBOT_X_MIN) return false;
    if(y>ROBOT_Y_MAX || y<ROBOT_Y_MIN) return false;
    if(z>ROBOT_Z_MAX || z<ROBOT_Z_MIN) return false;
    if(yaw>MAX_ROTATION || yaw<-1*MAX_ROTATION) return false;
    if(pitch>MAX_ROTATION || pitch<-1*MAX_ROTATION) return false;
    if(roll>MAX_ROTATION || roll<-1*MAX_ROTATION) return false;
    return true;
}

// Return how many items there are in the coordinate list
int Model::getDataAmount()
{
    return myCoordinates.getListSize();
}

// Sets the current state of the index to the point
void Model::setCurrentPoint(int index)
{
    currentPoint = index;
}

// Returns the integer of the currently selected state
int Model::getSelectedIndex()
{
    return currentPoint;
}

// Returns the currently selected coordinate class
coordinate Model::getSelectedCoordinate()
{
    return myCoordinates.getCoordinateAtPosition(currentPoint);
}

// Writes the content of the coordinate vector to a file
void Model::writeToFile(QString fileName){
    int count, temp;
    std::vector<coordinate> roughCoordinates = myCoordinates.getFullRoughCoordinates();
    float a, b, c, yaw, pitch, roll, t;
    if(fileName==NULL){
        fileName=trajDir;
    }
    QFile outputFile(fileName);
    outputFile.open((QIODevice::WriteOnly | QIODevice::Text));
    QTextStream out(&outputFile);
    out<<fileHeader;
    temp=roughCoordinates.size();
    for( count=0; count<temp; count++){
        a=roughCoordinates[count].x/1000;
        b=roughCoordinates[count].y/1000;
        c=roughCoordinates[count].z/1000;
        yaw=roughCoordinates[count].yaw;
        pitch=roughCoordinates[count].pitch;
        roll=roughCoordinates[count].roll;
        t=roughCoordinates[count].time;
        QString coor = "\n\t\t\t\t\t<q>"
                + QString::number(a) + " "
                + QString::number(b) + " "
                + QString::number(c) + " "
                + QString::number(yaw) + " "
                + QString::number(pitch) + " "
                + QString::number(roll)+
                "</q>";
        if(count==0){
            out<<openPoint<<">"<<coor<<qDot<<closePoint;
        }
        else{
            out<<openPoint<<" time=\""<<QString::number(t)<<".0\">"<<coor<<qDot<<closePoint;
        }

    }
    out<<fileCloser;
    outputFile.close();
    return;
}

// Returns the outer frame of the robot
std::vector<frame> Model::getFrame()
{
    return robotFrame;
}

// Makes the outer frame of the robot.
std::vector<frame> Model::makeFrame()
{
    // https://puu.sh/ymSp4/85b41d2c20.png
    frame frame1(glm::vec3(ROBOT_FRAME_SIZE,0.0,0.0),glm::vec3(ROBOT_FRAME_SIZE+FRAME_LENGTH,ROBOT_FRAME_SIZE,ROBOT_FRAME_SIZE));
    frame frame2(glm::vec3(FRAME_LENGTH,0.0,ROBOT_FRAME_SIZE),glm::vec3(ROBOT_FRAME_SIZE+FRAME_LENGTH,ROBOT_FRAME_SIZE,ROBOT_FRAME_SIZE+FRAME_LENGTH));
    frame frame3(glm::vec3(FRAME_LENGTH,0.0,FRAME_LENGTH),glm::vec3(0.0,ROBOT_FRAME_SIZE,FRAME_LENGTH+ROBOT_FRAME_SIZE));
    frame frame4(glm::vec3(ROBOT_FRAME_SIZE,0.0,FRAME_LENGTH),glm::vec3(0.0,ROBOT_FRAME_SIZE,0.0));
    frame frame5(glm::vec3(0.0,ROBOT_FRAME_SIZE,0.0),glm::vec3(ROBOT_FRAME_SIZE,ROBOT_FRAME_SIZE+ROBOT_HEIGHT,ROBOT_FRAME_SIZE));
    frame frame6(glm::vec3(FRAME_LENGTH,ROBOT_FRAME_SIZE,0.0),glm::vec3(FRAME_LENGTH+ROBOT_FRAME_SIZE,ROBOT_FRAME_SIZE+ROBOT_HEIGHT,ROBOT_FRAME_SIZE));
    frame frame7(glm::vec3(FRAME_LENGTH,ROBOT_FRAME_SIZE,FRAME_LENGTH),glm::vec3(FRAME_LENGTH+ROBOT_FRAME_SIZE,ROBOT_FRAME_SIZE+ROBOT_HEIGHT,ROBOT_FRAME_SIZE+ROBOT_HEIGHT));
    frame frame8(glm::vec3(0.0,ROBOT_FRAME_SIZE,FRAME_LENGTH),glm::vec3(ROBOT_FRAME_SIZE,ROBOT_FRAME_SIZE+ROBOT_HEIGHT,ROBOT_FRAME_SIZE+ROBOT_HEIGHT));
    frame frame9(glm::vec3(ROBOT_FRAME_SIZE,ROBOT_FRAME_SIZE+ROBOT_HEIGHT,0.0),glm::vec3(ROBOT_FRAME_SIZE+FRAME_LENGTH,ROBOT_FRAME_SIZE+ROBOT_FRAME_SIZE+ROBOT_HEIGHT,ROBOT_FRAME_SIZE));
    frame frame10(glm::vec3(FRAME_LENGTH,ROBOT_FRAME_SIZE+ROBOT_HEIGHT,ROBOT_FRAME_SIZE),glm::vec3(ROBOT_FRAME_SIZE+FRAME_LENGTH,ROBOT_FRAME_SIZE+ROBOT_FRAME_SIZE+ROBOT_HEIGHT,ROBOT_FRAME_SIZE+FRAME_LENGTH));
    frame frame11(glm::vec3(FRAME_LENGTH,ROBOT_FRAME_SIZE+ROBOT_HEIGHT,FRAME_LENGTH),glm::vec3(0.0,ROBOT_FRAME_SIZE+ROBOT_FRAME_SIZE+ROBOT_HEIGHT,FRAME_LENGTH+ROBOT_FRAME_SIZE));
    frame frame12(glm::vec3(ROBOT_FRAME_SIZE,ROBOT_FRAME_SIZE+ROBOT_HEIGHT,FRAME_LENGTH),glm::vec3(0.0,ROBOT_FRAME_SIZE+ROBOT_FRAME_SIZE+ROBOT_HEIGHT,0.0));
    std::vector<frame> frameParts;
    frameParts.push_back(frame1);
    frameParts.push_back(frame2);
    frameParts.push_back(frame3);
    frameParts.push_back(frame4);
    frameParts.push_back(frame5);
    frameParts.push_back(frame6);
    frameParts.push_back(frame7);
    frameParts.push_back(frame8);
    frameParts.push_back(frame9);
    frameParts.push_back(frame10);
    frameParts.push_back(frame11);
    frameParts.push_back(frame12);
    return frameParts;
}

// Returns the End Effector class
EndEffector *Model::getEndEffector()
{
    return &myEndEffector;
}

// Updates the End Effector coordinates based on the current position
void Model::updateEndEffector()
{
    coordinate currentCoordinate;
    if(playState==stop)
    {
        currentCoordinate = myCoordinates.getCoordinateAtPosition(currentPoint);

    }else
    {
        currentCoordinate = myCoordinates.getFineCoordinate((float)timer.elapsed()/1000);
    }
    myEndEffector.translatePosition(
        currentCoordinate.x,
        currentCoordinate.y,
        currentCoordinate.z,
        currentCoordinate.yaw,
        currentCoordinate.pitch,
        currentCoordinate.roll
    );

}

void Model::setPlay()
{
    timer.start();
    playState = play;
}

void Model::setStop()
{
    playState = stop;
}
