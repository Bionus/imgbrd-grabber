#include <QtGlobal>
#include "aboutwindow.h"
#include "ui_aboutwindow.h"



AboutWindow::AboutWindow(QString version, QWidget *parent)
	: QDialog(parent), ui(new Ui::AboutWindow), m_updater()
{
	ui->setupUi(this);

	ui->labelCurrent->setText(version);
	setFixedSize(400, 228);

	connect(&m_updater, &ProgramUpdater::finished, this, &AboutWindow::finished);
	m_updater.checkForUpdates();
}

AboutWindow::~AboutWindow()
{
	delete ui;
}

void AboutWindow::finished(QString newVersion, bool available)
{
	QString msg = available ? tr("A new version is available: %1").arg(newVersion) : tr("Grabber is up to date");
	ui->labelMessage->setText("<p style=\"font-size:8pt; font-style:italic; color:#808080;\">" + msg + "</p>");
}
