#include "aboutwindow.h"
#include "ui_aboutwindow.h"


AboutWindow::AboutWindow(const QString &version, QWidget *parent)
	: QDialog(parent), ui(new Ui::AboutWindow), m_updater()
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);

	ui->labelCurrent->setText(version);
	ui->labelCreator->setText(ui->labelCreator->text().replace("{website}", PROJECT_WEBSITE_URL));
	setFixedSize(400, 228);

	connect(&m_updater, &ProgramUpdater::finished, this, &AboutWindow::finished);
	m_updater.checkForUpdates();
}

AboutWindow::~AboutWindow()
{
	delete ui;
}

void AboutWindow::finished(const QString &newVersion, bool available)
{
	const QString msg = available ? tr("A new version is available: %1").arg(newVersion) : tr("Grabber is up to date");
	ui->labelMessage->setText("<p style=\"font-size:8pt; font-style:italic; color:#808080;\">" + msg + "</p>");
}
