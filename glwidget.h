#ifndef GLWIDGET_H
#define GLWIDGET_H
#include <QtOpenGL>
#include <QTimer>
#include "model.h"
#include "framestruct.h"
#include <GL/glu.h>
#include <vector>
#include "endeffector.h"
#include <glm/vec3.hpp>

class GLWidget : public QGLWidget
{
public:
    explicit GLWidget(QWidget *parent=0);
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int);
    void setModel(Model* newModel);
private:
    QTimer timer;
    Model* myModel;
    EndEffector* endEffector;
    void createAxisPaint();
    void createRobotFrame();
    void paintRobotEndEffector();
    void drawBox(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec3 p5, glm::vec3 p6, glm::vec3 p7, glm::vec3 p8 );
    void drawBox(EndEffector endEffector);
    std::vector<frame> robotFrame;
};

#endif // GLWIDGET_H
