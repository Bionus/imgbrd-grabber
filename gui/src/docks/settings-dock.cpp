#include "docks/settings-dock.h"
#include <QCompleter>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QSettings>
#include <QString>
#include <ui_settings-dock.h>
#include "functions.h"
#include "models/filename.h"
#include "models/profile.h"


SettingsDock::SettingsDock(Profile *profile, QWidget *parent)
	: QWidget(parent), ui(new Ui::SettingsDock), m_profile(profile), m_settings(profile->getSettings())
{
	ui->setupUi(this);

	reset();

	m_lineFolder_completer = QStringList(m_settings->value("Save/path").toString());
	ui->lineFolder->setCompleter(new QCompleter(m_lineFolder_completer, ui->lineFolder));
	// m_lineFilename_completer = QStringList(m_settings->value("Save/filename").toString());
	// ui->lineFilename->setCompleter(new QCompleter(m_lineFilename_completer));
	ui->comboFilename->setAutoCompletionCaseSensitivity(Qt::CaseSensitive);
}

SettingsDock::~SettingsDock()
{
	close();
	delete ui;
}

void SettingsDock::changeEvent(QEvent *event)
{
	if (event->type() == QEvent::LanguageChange) {
		ui->retranslateUi(this);
	}

	QWidget::changeEvent(event);
}

void SettingsDock::chooseFolder()
{
	QString folder = QFileDialog::getExistingDirectory(this, tr("Choose a save folder"), ui->lineFolder->text());
	if (!folder.isEmpty()) {
		ui->lineFolder->setText(folder);
		updateCompleters();
		saveSettings();
	}
}

void SettingsDock::reset()
{
	// Reload filename history
	QFile f(m_profile->getPath() + "/filenamehistory.txt");
	QStringList filenames;
	if (f.open(QFile::ReadOnly | QFile::Text)) {
		QString line;
		while ((line = f.readLine()) > 0) {
			QString l = line.trimmed();
			if (!l.isEmpty() && !filenames.contains(l)) {
				filenames.append(l);
				ui->comboFilename->addItem(l);
			}
		}
		f.close();
	}

	// Update quick settings dock
	ui->lineFolder->setText(m_settings->value("Save/path_real").toString());
	ui->comboFilename->setCurrentText(m_settings->value("Save/filename_real").toString());

	// Save settings
	saveSettings();
}

void SettingsDock::save()
{
	QString folder = fixFilename("", ui->lineFolder->text());
	if (!QDir(folder).exists()) {
		QDir::root().mkpath(folder);
	}

	m_settings->setValue("Save/path_real", folder);
	m_settings->setValue("Save/filename_real", ui->comboFilename->currentText());
	saveSettings();
}

void SettingsDock::updateCompleters()
{
	if (ui->lineFolder->text() != m_settings->value("Save/path").toString()) {
		m_lineFolder_completer.append(ui->lineFolder->text());
		ui->lineFolder->setCompleter(new QCompleter(m_lineFolder_completer));
	}
	/*if (ui->labelFilename->text() != m_settings->value("Save/filename").toString()) {
		m_lineFilename_completer.append(ui->lineFilename->text());
		ui->lineFilename->setCompleter(new QCompleter(m_lineFilename_completer));
	}*/
}

void SettingsDock::saveSettings()
{
	// Filename combobox
	QString txt = ui->comboFilename->currentText();
	for (int i = ui->comboFilename->count() - 1; i >= 0; --i) {
		if (ui->comboFilename->itemText(i) == txt) {
			ui->comboFilename->removeItem(i);
		}
	}
	ui->comboFilename->insertItem(0, txt);
	ui->comboFilename->setCurrentIndex(0);
	QString message;
	Filename fn(ui->comboFilename->currentText());
	fn.isValid(m_profile, &message);
	ui->labelFilename->setText(message);

	// Save filename history
	QFile f(m_profile->getPath() + "/filenamehistory.txt");
	if (f.open(QFile::WriteOnly | QFile::Text | QFile::Truncate)) {
		for (int i = qMax(0, ui->comboFilename->count() - 50); i < ui->comboFilename->count(); ++i) {
			f.write(QString(ui->comboFilename->itemText(i) + "\n").toUtf8());
		}
		f.close();
	}

	// Update settings
	QString folder = fixFilename("", ui->lineFolder->text());
	m_settings->setValue("Save/path", folder);
	m_settings->setValue("Save/filename", ui->comboFilename->currentText());
	m_settings->sync();
}
