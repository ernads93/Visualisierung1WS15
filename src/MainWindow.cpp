#include "MainWindow.h"

#include <QFileDialog>

#include <QPainter>


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), m_Volume(0), m_VectorField(0)
{
	m_Ui = new Ui_MainWindow();
	m_Ui->setupUi(this);

	connect(m_Ui->actionOpen, SIGNAL(triggered()), this, SLOT(openFileAction()));
	connect(m_Ui->actionClose, SIGNAL(triggered()), this, SLOT(closeAction()));
	connect(m_Ui->radioFH, SIGNAL(clicked()), this, SLOT(chooseRenderingTechnique()));
	connect(m_Ui->radioMIP, SIGNAL(clicked()), this, SLOT(chooseRenderingTechnique()));
	connect(m_Ui->renderButton, SIGNAL(clicked()), this, SLOT(startRendering()));
	connect(m_Ui->sampleSlider, SIGNAL(valueChanged(int)), this, SLOT(setSampleDistance(int)));

	/*connect(m_Ui->xRotSlider, SIGNAL(valueChanged(int)), m_Ui->myGLWidget, SLOT(setXRotation(int)));
	connect(m_Ui->yRotSlider, SIGNAL(valueChanged(int)), m_Ui->myGLWidget, SLOT(setYRotation(int)));
	connect(m_Ui->zRotSlider, SIGNAL(valueChanged(int)), m_Ui->myGLWidget, SLOT(setYRotation(int)));*/
}

MainWindow::~MainWindow()
{
	delete m_Volume;
	delete m_VectorField;
	delete m_MultiSet;
}


//-------------------------------------------------------------------------------------------------
// Slots
//-------------------------------------------------------------------------------------------------

void MainWindow::openFileAction()
{
	QString filename = QFileDialog::getOpenFileName(this, "Data File", 0, tr("Data Files (*.dat *.gri *.csv)"));

	if (!filename.isEmpty())
	{
		// store filename
		m_FileType.filename = filename;
		std::string fn = filename.toStdString();
		bool success = false;

		// progress bar and top label
		m_Ui->progressBar->setEnabled(true);
		m_Ui->labelTop->setText("Loading data ...");

		// load data according to file extension
		if (fn.substr(fn.find_last_of(".") + 1) == "dat")		// LOAD VOLUME      <----------------------
		{
			// create VOLUME
			m_FileType.type = VOLUME;
			m_Volume = new Volume();

			// load file
			success = m_Volume->loadFromFile(filename, m_Ui->progressBar);

			if (success) {
				//m_Volume->setFirstHit();
				m_Ui->myGLWidget->setVolume(m_Volume);
			}

		}
		else if (fn.substr(fn.find_last_of(".") + 1) == "gri")		// LOAD VECTORFIELD
		{
			// create VECTORFIELD
			m_FileType.type = VECTORFIELD;
			m_VectorField = new VectorField();

			// load file
			success = m_VectorField->loadFromFile(filename, m_Ui->progressBar);
		}
		else if (fn.substr(fn.find_last_of(".") + 1) == "csv")		// LOAD MULTIVARIATE DATA
		{
			// create MULTIVARIATE
			m_FileType.type = MULTIVARIATE;
			m_MultiSet = new MultiSet();

			// load file
			success = m_MultiSet->loadFromFile(filename, m_Ui->progressBar);
		}

		m_Ui->progressBar->setEnabled(false);

		// status message
		if (success)
		{
			QString type;
			if (m_FileType.type == VOLUME) type = "VOLUME";
			else if (m_FileType.type == VECTORFIELD) type = "VECTORFIELD";
			else if (m_FileType.type == MULTIVARIATE) type = "MULTIVARIATE";
			m_Ui->labelTop->setText("File LOADED [" + filename + "] - Type [" + type + "]");
		}
		else
		{
			m_Ui->labelTop->setText("ERROR loading file " + filename + "!");
			m_Ui->progressBar->setValue(0);
		}

	}
}

void MainWindow::closeAction()
{
	close();
}

void MainWindow::chooseRenderingTechnique()
{
	if (m_Ui->radioMIP->isChecked())
	{
		std::cout << "set rendering technique MIP" << std::endl;
		m_Volume->setMip();
	}

	if (m_Ui->radioFH->isChecked())
	{
		std::cout << "set rendering technique first hit" << std::endl;
		m_Volume->setFirstHit();
	}
}

void MainWindow::setSampleDistance(int distance)
{
	std::cout << "set sample distance: " << distance << std::endl;
	m_Volume->setSampleDistance(distance);
}

void MainWindow::startRendering()
{
	m_Ui->myGLWidget->startRendering();
}
