#include "settings/start-window.h"
#include <QFileDialog>
#include <QSettings>
#include <QStandardPaths>
#include <ui_start-window.h>
#include "functions.h"
#include "helpers.h"
#include "language-loader.h"
#include "models/profile.h"
#include "settings/filename-window.h"
#include "settings/options-window.h"


StartWindow::StartWindow(Profile *profile, QWidget *parent)
	: QDialog(parent), ui(new Ui::StartWindow), m_profile(profile)
{
	ui->setupUi(this);
	ui->labelHelp->setText(ui->labelHelp->text().replace("{website}", PROJECT_WEBSITE_URL));
	ui->labelHelp->setText(ui->labelHelp->text().replace("{github}", PROJECT_GITHUB_URL));

	// Language
	LanguageLoader languageLoader(savePath("languages/", true));
	QMap<QString, QString> languages = languageLoader.getAllLanguages();
	for (auto it = languages.constBegin(); it != languages.constEnd(); ++it) {
		ui->comboLanguage->addItem(it.value(), it.key());
	}
	ui->comboLanguage->setCurrentText("English");

	// Sources
	QStringList sources = profile->getSites().keys();
	ui->comboSource->addItems(sources);
	if (sources.contains("danbooru.donmai.us")) {
		ui->comboSource->setCurrentText("danbooru.donmai.us");
	}

	// Default values
	QDir desktop(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
	ui->lineFolder->setText(desktop.absoluteFilePath("Grabber"));
	ui->lineFilename->setText("%md5%.%ext%");

	connect(this, &QDialog::accepted, this, &StartWindow::save);
}

/**
 * Destructor of the StartWindow class.
 */
StartWindow::~StartWindow()
{
	delete ui;
}

void StartWindow::on_buttonFolder_clicked()
{
	QString folder = QFileDialog::getExistingDirectory(this, tr("Choose a save folder"), ui->lineFolder->text());
	if (!folder.isEmpty()) {
		ui->lineFolder->setText(folder);
	}
}
void StartWindow::on_buttonFilenamePlus_clicked()
{
	FilenameWindow *fw = new FilenameWindow(m_profile, ui->lineFilename->text(), this);
	connect(fw, &FilenameWindow::validated, ui->lineFilename, &QLineEdit::setText);
	fw->show();
}

/**
 * Save initial settings.
 */
void StartWindow::save()
{
	QSettings *settings = m_profile->getSettings();
	settings->beginGroup("Save");

	// Filename
	settings->setValue("filename", ui->lineFilename->text());
	settings->setValue("filename_real", ui->lineFilename->text());

	// Folder
	settings->setValue("path", ui->lineFolder->text());
	settings->setValue("path_real", ui->lineFolder->text());
	QDir pth = QDir(ui->lineFolder->text());
	if (!pth.exists()) {
		QString op;
		while (!pth.exists() && pth.path() != op) {
			op = pth.path();
			pth.setPath(pth.path().remove(QRegularExpression("/([^/]+)$")));
		}
		if (pth.path() == op) {
			error(this, tr("An error occurred creating the save folder."));
		} else {
			pth.mkpath(ui->lineFolder->text());
		}
	}
	settings->endGroup();

	// Language
	QString lang = ui->comboLanguage->currentData().toString();
	if (settings->value("language", "English").toString() != lang) {
		settings->setValue("language", lang);
		emit languageChanged(lang);
	}

	emit sourceChanged(ui->comboSource->currentText());

	settings->sync();
	emit settingsChanged();
}

/**
 * Open a full settings window.
 */
void StartWindow::openOptions()
{
	auto *ow = new OptionsWindow(m_profile, parentWidget());
	ow->show();

	this->close();
}
