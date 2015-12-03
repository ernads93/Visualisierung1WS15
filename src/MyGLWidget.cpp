#include "MyGLWidget.h"

MyGLWidget::MyGLWidget(QWidget *parent) :
QGLWidget(QGLFormat(), parent)
{
	fileLoaded = false;
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(50);
}

MyGLWidget::~MyGLWidget()
{

}

void MyGLWidget::initializeGL()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	qglClearColor(QColor(Qt::black));
	shaderProgram.addShaderFromSourceFile(QGLShader::Vertex, "shader/simple.vert");
	shaderProgram.addShaderFromSourceFile(QGLShader::Fragment, "shader/simple.frag");

	shaderProgram.link();
	if (!shaderProgram.bind()){
		qWarning() << "Couldnt bind shader program";
		return;
	}
	vertices << QVector3D(1, 0, -2) << QVector3D(0, 1, -2) << QVector3D(-1, 0, -2);
}

void MyGLWidget::paintGL()
{
	if (fileLoaded){
		//volume->rotate(.1f);
		data = volume->getVolume();
		std::cout << "Test: " << data[0] << std::endl;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		float* pixel = &data[0];
		glDrawPixels(640, 480, GL_LUMINANCE, GL_FLOAT, pixel);
		shaderProgram.release();
	}
}

QSize MyGLWidget::sizeHint() const
{
	return QSize(640, 480);
}

void MyGLWidget::resizeGL(int w, int h)
{
	if (h == 0){
		h = 1;
	}
	pMatrix.setToIdentity();
	pMatrix.perspective(60.f, (float)w / (float)h, .2f, 10.f);
	glViewport(0, 0, w, h);
}

void MyGLWidget::setVolume(Volume* v)
{
	this->volume = v;
}