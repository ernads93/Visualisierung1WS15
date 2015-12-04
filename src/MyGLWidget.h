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

	bool success;
	Volume* volume;

	void setVolume(Volume* v);

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);

	QSize minimumSizeHint() const;
	QSize sizeHint() const;
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

public slots:
	// slots for xyz-rotation slider
	void setXRotation(int angle);
	void setYRotation(int angle);
	void setZRotation(int angle);

signals:
	// signaling rotation from mouse movement
	void xRotationChanged(int angle);
	void yRotationChanged(int angle);
	void zRotationChanged(int angle);

private:

	void draw();

	int xRot;
	int yRot;
	int zRot;

	QPoint lastPos;

};
#endif