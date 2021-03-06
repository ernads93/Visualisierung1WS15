
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_MainWindow.h"
#include "Volume.h"
#include "VectorField.h"
#include "MultiSet.h"

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QStatusBar>
#include <QVariant>


class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:

		MainWindow(QWidget *parent = 0);
		~MainWindow();

	
	protected slots :

		void			openFileAction();
		void			closeAction();
		void			chooseRenderingTechnique();
		void			setSampleSlider(int distance);
		void			setSampleDistance();
		void			startRendering();
		void			setTransSlider(int alpha);
		void			setTransAlpha();
		void			setScaleSlider(int factor);
		void			setScaleFactor();
		

	private:

		// USER INTERFACE ELEMENTS

		Ui_MainWindow						*m_Ui;


		// DATA 

		enum DataType
		{
			VOLUME					= 0,
			VECTORFIELD				= 1,
			MULTIVARIATE			= 2
		};

		struct FileType
		{
			QString			filename;
			DataType		type;
		}									 m_FileType;

		Volume				*m_Volume;						// for Volume-Rendering
		VectorField			*m_VectorField;					// for Flow-Visualisation
		MultiSet			*m_MultiSet;					// for Multivariate Data

		bool				success = false;
		int					m_sample;
		int					m_alpha;
		int					m_factor;

};

#endif
