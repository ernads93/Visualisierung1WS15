#include <QtWidgets>
#include <QtOpenGL>

#include "MyGLWidget.h"

MyGLWidget::MyGLWidget(QWidget *parent) :
QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
	success = false;
	start = false;
}

MyGLWidget::~MyGLWidget()
{

}

QSize MyGLWidget::sizeHint() const
{
	return QSize(640, 480);
}

void MyGLWidget::initializeGL()
{
	qglClearColor(Qt::black);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	static GLfloat lightPosition[4] = { 0, 0, 10, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

}

void MyGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	if (success)
	{
		std::cout << "MyGLWidget start raycasting" << std::endl;
		
		std::vector<float> pixels = volume->rayCasting2();

		int width = volume->width() * volume->getScaleFactor();
		int height = volume->height() * volume->getScaleFactor();

		glDrawPixels(width, height, GL_LUMINANCE, GL_FLOAT, &pixels[0]);

		std::cout << "MyGLWidget end raycasting" << std::endl;	

		start = false;
	}
}

void MyGLWidget::setVolume(Volume* v)
{
	std::cout << "MyGLWidget set Volume" << std::endl;
	this->volume = v;
	
	success = true;
}

void MyGLWidget::startRendering()
{
	start = true;
	paintGL();
}