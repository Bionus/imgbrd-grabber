#include "startwindow.h"
#include "optionswindow.h"



/**
 * Constructor of the startWindow class, completing its window.
 * @param	parent		The parent window
 */
startWindow::startWindow(mainWindow *parent) : QDialog(parent), m_parent(parent), ui(new Ui::startWindow)
{
	ui->setupUi(this);
	connect(this, SIGNAL(accepted()), this, SLOT(openOptions()));
}

/**
 * Destructor of the startWindow class
 */
startWindow::~startWindow()
{
	delete ui;
}

/**
 * Save settings in the settings.ini file
 */
void startWindow::openOptions()
{
	optionsWindow *ow = new optionsWindow(m_parent);
	ow->show();
	ow->setCategory(tr("Sauvegarde", "update"));
}
