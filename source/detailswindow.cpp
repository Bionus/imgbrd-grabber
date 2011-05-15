#include "detailswindow.h"
#include "ui_detailsWindow.h"



/**
 * Constructor of the detailsWindow class, completing its window.
 * @param	tags		Image's tags (colored or not)
 */
detailsWindow::detailsWindow(QString tags) : QWidget(0), ui(new Ui::detailsWindow)
{
	ui->setupUi(this);
	if (!tags.isEmpty())	{ ui->labelTags->setText(tags);	}
}

/**
 * Destructor of the detailsWindow class
 */
detailsWindow::~detailsWindow()
{
	delete ui;
}
