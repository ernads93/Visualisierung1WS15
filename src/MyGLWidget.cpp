#include <QtWidgets>
#include <QtOpenGL>

#include "MyGLWidget.h"

MyGLWidget::MyGLWidget(QWidget *parent) :
QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
	success = false;

	xRot = 0;
	yRot = 0;
	zRot = 0;
}

MyGLWidget::~MyGLWidget()
{

}
/*QSize MyGLWidget::minimumSizeHint() const
{
	return QSize(50, 50);
}*/

QSize MyGLWidget::sizeHint() const
{
	return QSize(640, 480);
}

static void qNormalizeAngle(int &angle)
{
	while (angle < 0)
		angle += 360 * 16;
	while (angle > 360)
		angle -= 360 * 16;
}

void MyGLWidget::setXRotation(int angle)
{
	//std::cout << "set X Rotation" << std::endl;
	qNormalizeAngle(angle);
	if (angle != xRot) {
		xRot = angle;
		//emit xRotationChanged(angle);
		updateGL();
	}
}

void MyGLWidget::setYRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != yRot) {
		yRot = angle;
		//emit yRotationChanged(angle);
		updateGL();
	}
}

void MyGLWidget::setZRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != zRot) {
		zRot = angle;
		//emit zRotationChanged(angle);
		updateGL();
	}
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

	shader.addShaderFromSourceFile(QGLShader::Vertex, "shader/simple.vert");
	shader.addShaderFromSourceFile(QGLShader::Fragment, "shader/simple.frag");

	shader.link();
	if (!shader.bind()){
		qWarning() << "Couldnt bind shader program";
		return;
	}
}

void MyGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -10.0);
	glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
	glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);
	glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);

	if (success)
	{
		std::cout << "MyGLWidget start raycasting" << std::endl;
		
		std::vector<float> pixels = volume->rayCasting();
		
		std::cout << "MyGLWidget end raycasting" << std::endl;

		int width = volume->width();
		int height = volume->height();

		//glPixelZoom(640 / width, 480 / height);
		glDrawPixels(width, height, GL_LUMINANCE, GL_FLOAT, &pixels[0]);
		shader.release();
	}
}

/*void MyGLWidget::resizeGL(int width, int height)
{
	int side = qMin(width, height);
	glViewport((width - side) / 2, (height - side) / 2, side, side);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
#ifdef QT_OPENGL_ES_1
	glOrthof(-2, +2, -2, +2, 1.0, 15.0);
#else
	glOrtho(-2, +2, -2, +2, 1.0, 15.0);
#endif
	glMatrixMode(GL_MODELVIEW);
}*/

void MyGLWidget::mousePressEvent(QMouseEvent *event)
{
	lastPos = event->pos();
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event)
{
	int dx = event->x() - lastPos.x();
	int dy = event->y() - lastPos.y();

	if (event->buttons() & Qt::LeftButton) {
		setXRotation(xRot + 8 * dy);
		setYRotation(yRot + 8 * dx);
	}
	else if (event->buttons() & Qt::RightButton) {
		setXRotation(xRot + 8 * dy);
		setZRotation(zRot + 8 * dx);
	}

	lastPos = event->pos();
}

void MyGLWidget::setVolume(Volume* v)
{
	std::cout << "MyGLWidget set Volume" << std::endl;
	this->volume = v;
	
	success = true;

	//this->data = v->getVolume();
}