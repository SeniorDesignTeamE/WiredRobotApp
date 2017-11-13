#include "glwidget.h"
#include <QtDebug>

GLWidget::GLWidget(QWidget *parent) :
    QGLWidget(parent)
{
    qDebug() <<"parent called";
}

void GLWidget::initializeGL()
{
    glClearColor(0.2,0.2,0.2,1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    glColor3f(1,0,0);
    glVertex3f(-1,0,0);
    glVertex3f(1,0,0);
    glVertex3f(0,-1,0);
    glEnd();

}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    updateGL();
}
