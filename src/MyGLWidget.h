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

	Volume* volume;

	void setVolume(Volume* v);
	void startRendering();

protected:
	void initializeGL();
	void paintGL();

	QSize sizeHint() const;

private:
	bool success;
	bool start;

};
#endif