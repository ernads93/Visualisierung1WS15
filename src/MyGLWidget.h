#pragma once
#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include <QtOpenGL/QGLWidget>
#include <QWidget>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QWidget>
#include <QtOpenGL>
#include <QGLShaderProgram>
#include <vector>
#include "Volume.h"

class MyGLWidget : public QGLWidget
{

public:
	MyGLWidget(QWidget *parent = 0);
	~MyGLWidget();

	QSize sizeHint() const;
	bool fileLoaded;
	bool ready;
	Volume* volume;
	std::vector<float> data;
	float pixel[640 * 480];

	void setVolume(Volume* v);

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

private:
	QMatrix4x4 pMatrix;
	QGLShaderProgram shaderProgram;
	QVector<QVector3D> vertices;
};
#endif