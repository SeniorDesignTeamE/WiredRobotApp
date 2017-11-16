#ifndef MODEL_H
#define MODEL_H
#include "coordinatestruct.h"
#include <fstream>
#include <iterator>
#include <vector>
#include <QString>
#include "framestruct.h"

class Model
{
public:
    Model();
    void setCurrentPoint(int index);
    bool pushNewPoint(float x,float y,float z,float yaw,float pitch,float roll,float time);
    bool insertNewPoint(float x, float y, float z, float yaw, float pitch, float roll,float time);
    int getSelectedIndex();
    int getDataAmount();
    void writeData(float x, float y, float z, float yaw, float pitch, float roll,float time);
    coordinate getSelectedCoordinate();
    bool deleteCurrentIdex();
    bool emptyWorkingPoints();
    void writeToFile(QString fileName);
    std::vector<frame> getFrame();

    const float FRAME_LENGTH = 2000;
    const float ROBOT_HEIGHT = 2100; // TODO: Measure this out
    const float ROBOT_FRAME_SIZE = 20;


private:
    bool isPointValid(float x, float y, float z, float yaw, float pitch, float roll);
    std::vector<coordinate> coordinateList;
    int currentPoint = 0;
    std::vector<frame> makeFrame();
    std::vector<frame> robotFrame;

    const float ROBOT_AREA_INFILL = 0.8;
    const float ROBOT_X_MIN = (1-ROBOT_AREA_INFILL)/2*FRAME_LENGTH;
    const float ROBOT_Y_MIN = (1-ROBOT_AREA_INFILL)/2*ROBOT_HEIGHT;
    const float ROBOT_Z_MIN = (1-ROBOT_AREA_INFILL)/2*FRAME_LENGTH;
    const float ROBOT_X_MAX = FRAME_LENGTH - ROBOT_X_MIN;
    const float ROBOT_Y_MAX = ROBOT_HEIGHT - ROBOT_Y_MIN;
    const float ROBOT_Z_MAX = FRAME_LENGTH - ROBOT_Z_MIN;
    const float MAX_ROTATION = 30.1;
    const float END_EFFECTOR = 100;

};

#endif // MODEL_H
