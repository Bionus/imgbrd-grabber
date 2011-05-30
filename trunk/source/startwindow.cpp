#include "startwindow.h"
#include "functions.h"



/**
 * Constructor of the startWindow class, completing its window.
 * @param	parent		The parent window
 */
startWindow::startWindow(mainWindow *parent) : QDialog(parent), m_parent(parent), ui(new Ui::startWindow)
{
	QSettings *settings = new QSettings(savePath("settings.ini"), QSettings::IniFormat);
	ui->setupUi(this);
	ui->pathLineEdit->setText(settings->value("Save/path").toString());
	ui->filenameLineEdit->setText(settings->value("Save/filename").toString());
	connect(this, SIGNAL(accepted()), this, SLOT(save()));
}

/**
 * Destructor of the startWindow class
 */
startWindow::~startWindow()
{
	delete ui;
}

/**
 * Opens a window to choose an folder to set the path value.
 */
void startWindow::on_openButton_clicked()
{
	QString folder = QFileDialog::getExistingDirectory(this, tr("Choisir un dossier"), QSettings(savePath("settings.ini"), QSettings::IniFormat).value("Save/path").toString());
	if (!folder.isEmpty())
	{ ui->pathLineEdit->setText(folder); }
}

/**
 * Checks filename's format validity and displays error or success message.
 */
void startWindow::on_filenameLineEdit_textChanged(QString text)
{ ui->validatorLabel->setText(validateFilename(text)); }

/**
 * Save settings in the settings.ini file
 */
void startWindow::save()
{
	QSettings *settings = new QSettings(savePath("settings.ini"), QSettings::IniFormat);
	settings->setValue("Save/path", ui->pathLineEdit->text());
	settings->setValue("Save/filename", ui->filenameLineEdit->text());
}
