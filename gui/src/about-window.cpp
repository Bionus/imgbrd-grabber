#include "about-window.h"
#include "ui_about-window.h"


AboutWindow::AboutWindow(const QString &version, QWidget *parent)
	: QDialog(parent), ui(new Ui::AboutWindow)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);

	// Set window title according to the current build
	QString labelVersion = version;
	#ifdef NIGHTLY
		QString commit(NIGHTLY_COMMIT);
		if (!commit.isEmpty()) {
			labelVersion += QString(" - nightly (%1)").arg(commit.left(8));
		} else {
			labelVersion += " - nightly";
		}
	#endif
	ui->labelCurrent->setText(labelVersion);

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
