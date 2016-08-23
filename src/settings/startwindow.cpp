#include "startwindow.h"
#include "ui_startwindow.h"
#include "optionswindow.h"
#include "filenamewindow.h"
#include "mainwindow.h"
#include "functions.h"
#include <QFileDialog>
#include <QSettings>
#include <QDesktopServices>



/**
 * Constructor of the startWindow class, completing its window.
 *
 * @param parent	The parent window
 */
startWindow::startWindow(QMap<QString, Site*> *sites, mainWindow *parent)
    : QDialog(parent), ui(new Ui::startWindow), m_parent(parent), m_sites(sites)
{
	ui->setupUi(this);

	// Language
	QStringList languages = QDir(savePath("languages/", true)).entryList(QStringList("*.qm"), QDir::Files);
	for (int i = 0; i < languages.count(); i++)
	{ languages[i].remove(".qm", Qt::CaseInsensitive); }
	ui->comboLanguage->addItems(languages);
	if (languages.contains("English"))
	{ ui->comboLanguage->setCurrentText("English"); }

	// Sources
	QStringList sources = m_sites->keys();
	ui->comboSource->addItems(sources);
	if (sources.contains("danbooru.donmai.us"))
	{ ui->comboSource->setCurrentText("danbooru.donmai.us"); }

	// Default values
	QDir desktop(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
	ui->lineFolder->setText(desktop.absoluteFilePath("Grabber"));
	ui->lineFilename->setText("%md5%.%ext%");

	connect(this, &QDialog::accepted, this, &startWindow::save);
}

/**
 * Destructor of the startWindow class.
 */
startWindow::~startWindow()
{
	delete ui;
}

void startWindow::on_buttonFolder_clicked()
{
	QString folder = QFileDialog::getExistingDirectory(this, tr("Choisir un dossier de sauvegarde"), ui->lineFolder->text());
	if (!folder.isEmpty())
	{ ui->lineFolder->setText(folder); }
}
void startWindow::on_buttonFilenamePlus_clicked()
{
	FilenameWindow *fw = new FilenameWindow(ui->lineFilename->text(), this);
	connect(fw, SIGNAL(validated(QString)), ui->lineFilename, SLOT(setText(QString)));
	fw->show();
}

/**
 * Save initial settings.
 */
void startWindow::save()
{
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
	settings.beginGroup("Save");

	// Filename
	settings.setValue("filename", ui->lineFilename->text());
	settings.setValue("filename_real", ui->lineFilename->text());

	// Folder
	settings.setValue("path", ui->lineFolder->text());
	settings.setValue("path_real", ui->lineFolder->text());
	QDir pth = QDir(ui->lineFolder->text());
	if (!pth.exists())
	{
		QString op = "";
		while (!pth.exists() && pth.path() != op)
		{
			op = pth.path();
			pth.setPath(pth.path().remove(QRegExp("/([^/]+)$")));
		}
		if (pth.path() == op)
		{ error(this, tr("Une erreur est survenue lors de la création du dossier de sauvegarde.")); }
		else
		{ pth.mkpath(ui->lineFolder->text()); }
	}
	settings.endGroup();

	// Language
	QString lang = ui->comboLanguage->currentText();
	if (settings.value("language", "English").toString() != lang)
	{
		settings.setValue("language", lang);
		emit languageChanged(lang);
	}

	emit sourceChanged(ui->comboSource->currentText());

	settings.sync();
	emit settingsChanged();
}

/**
 * Open a full settings window.
 */
void startWindow::openOptions()
{
	optionsWindow *ow = new optionsWindow(m_parent);
	ow->show();

	this->close();
}
