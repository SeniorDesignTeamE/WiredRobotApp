#ifndef GLWIDGET_H
#define GLWIDGET_H
#include <QtOpenGL>
#include <QTimer>
#include "model.h"
#include "framestruct.h"
#include <GL/glu.h>
#include <vector>

class GLWidget : public QGLWidget
{
public:
    explicit GLWidget(QWidget *parent=0);

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void setModel(Model* newModel);
private:
    QTimer timer;
    Model* myModel;
    void createAxisPaint();
    void createRobotFrame();
    void drawBox(float x1, float y1,float z1, float x2, float y2, float z2);
    std::vector<frame> robotFrame;
};

#endif // GLWIDGET_H
