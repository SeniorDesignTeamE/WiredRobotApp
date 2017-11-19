#ifndef COORDINATES_H
#define COORDINATES_H
#include <vector>
#include "coordinatestruct.h"

class Coordinates
{
public:
    Coordinates();
    Coordinates(float xmin, float ymin, float zmin);
    void pushNewCoordinate(float x, float y, float z, float yaw, float pitch, float roll, float time);
    void insertNewCoordinate(float x, float y, float z, float yaw, float pitch, float roll, float time,int position);
    void deleteIndex(int i);
    coordinate getCoordinateAtPosition(int i);
    int getListSize();
    void clearAllPoints();
    std::vector<coordinate> getFullRoughCoordinates();
    coordinate getFineCoordinate(float time);
private:
    const float FRAME_FREQ = 0.02f; // Seconds per Frame
    void updateFineCoordinateList();
    std::vector<coordinate> roughCoordinateList;
    std::vector<coordinate> fineCoordinateList;
};

#endif // COORDINATES_H
