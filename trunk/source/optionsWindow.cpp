#include "optionswindow.h"
#include "ui_optionswindow.h"
#include "functions.h"



optionsWindow::optionsWindow(mainWindow *parent) : QDialog(parent), m_parent(parent), ui(new Ui::optionsWindow)
{
    ui->setupUi(this);
	for (int i = 1; i < ui->container->count(); i++)
	{ ui->container->itemAt(i)->widget()->hide(); }
	resize(QSize(600, 400));

	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
	QStringList languages = QDir("languages").entryList(QStringList("*.qm"), QDir::Files);
	for (int i = 0; i < languages.count(); i++)
	{ languages[i].remove(".qm", Qt::CaseInsensitive); }
	ui->comboLanguages->addItems(languages);

	ui->comboLanguages->setCurrentIndex(languages.indexOf(settings.value("language", "English").toString()));
	ui->spinCheckForUpdates->setValue(settings.value("updatesrate", 86400).toInt());
	ui->lineDateFormat->setText(settings.value("dateformat").toString());
	ui->lineBlacklist->setText(settings.value("blacklistedtags").toString());
	ui->checkDownloadBlacklisted->setChecked(settings.value("downloadblacklist", false).toBool());
	ui->checkLoadFirstAtStart->setChecked(settings.value("loadatstart", false).toBool());

	ui->spinImagesPerPage->setValue(settings.value("limit", 20).toInt());
	ui->spinColumns->setValue(settings.value("columns", 1).toInt());
	QStringList sources = QStringList() << "xml" << "json" << "regex";
	ui->comboSource1->setCurrentIndex(sources.indexOf(settings.value("source_1", "xml").toString()));
	ui->comboSource2->setCurrentIndex(sources.indexOf(settings.value("source_2", "xml").toString()));
	ui->comboSource3->setCurrentIndex(sources.indexOf(settings.value("source_3", "xml").toString()));

	ui->checkShowLog->setChecked(settings.value("Log/show", true).toBool());
	ui->checkInvertLog->setChecked(settings.value("Log/invert", false).toBool());

	settings.beginGroup("Save");

		ui->checkDownloadOriginals->setChecked(settings.value("downloadoriginals", true).toBool());
		ui->lineFolder->setText(settings.value("path").toString());
		ui->lineFilename->setText(settings.value("filename").toString());
		ui->lineSeparator->setText(settings.value("separator").toString());

		ui->lineArtistsIfNone->setText(settings.value("artist_empty", "anonymous").toString());
		ui->checkArtistsKeepAll->setChecked(settings.value("artist_useall", false).toBool());
		ui->lineArtistsSeparator->setText(settings.value("artist_sep", "+").toString());
		ui->lineArtistsIfMultiples->setText(settings.value("artist_value", "multiple artists").toString());

		ui->lineCopyrightsIfNone->setText(settings.value("copyright_empty", "misc").toString());
		ui->checkCopyrightsUseShorter->setChecked(settings.value("copyright_useshorter", true).toBool());
		ui->checkCopyrightsKeepAll->setChecked(settings.value("copyright_useall", false).toBool());
		ui->lineCopyrightsSeparator->setText(settings.value("copyright_sep", "+").toString());
		ui->lineCopyrightsIfMultiples->setText(settings.value("copyright_value", "crossover").toString());

		ui->lineCharactersIfNone->setText(settings.value("character_empty", "unknown").toString());
		ui->checkCharactersKeepAll->setChecked(settings.value("character_useall", false).toBool());
		ui->lineCharactersSeparator->setText(settings.value("character_sep", "+").toString());
		ui->lineCharactersIfMultiples->setText(settings.value("character_value", "group").toString());

	settings.endGroup();

	settings.beginGroup("Coloring");

		ui->lineColoringArtists->setText(settings.value("artists", "#aa0000").toString());
		ui->lineColoringCopyrights->setText(settings.value("copyrights", "#aa00aa").toString());
		ui->lineColoringCharacters->setText(settings.value("characters", "#00aa00").toString());

	settings.endGroup();

	settings.beginGroup("Exec");

		ui->lineCommandsInitialisation->setText(settings.value("init").toString());
		ui->lineCommandsImage->setText(settings.value("image").toString());
		ui->lineCommandsTag->setText(settings.value("tag").toString());

	settings.endGroup();

	connect(this, SIGNAL(accepted()), this, SLOT(save()));
}

optionsWindow::~optionsWindow()
{
	delete ui;
}

void optionsWindow::on_buttonFolder_clicked()
{
	QString folder = QFileDialog::getExistingDirectory(this, tr("Choisir un dossier de sauvegarde"), ui->lineFolder->text());
	if (!folder.isEmpty())
	{ ui->lineFolder->setText(folder); }
}
void optionsWindow::on_buttonColoringArtists_clicked()
{
	QColor color = QColorDialog::getColor(QColor(ui->lineColoringArtists->text()), this, "Grabber - Choisir une couleur");
	if (color.isValid())
	{ ui->lineColoringArtists->setText(color.name()); }
}
void optionsWindow::on_buttonColoringCopyrights_clicked()
{
	QColor color = QColorDialog::getColor(QColor(ui->lineColoringCopyrights->text()), this, "Grabber - Choisir une couleur");
	if (color.isValid())
	{ ui->lineColoringCopyrights->setText(color.name()); }
}
void optionsWindow::on_buttonColoringCharacters_clicked()
{
	QColor color = QColorDialog::getColor(QColor(ui->lineColoringCharacters->text()), this, "Grabber - Choisir une couleur");
	if (color.isValid())
	{ ui->lineColoringCharacters->setText(color.name()); }
}

void optionsWindow::updateContainer(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	QStringList texts = QStringList() <<
		tr("Général") <<
		tr("Sources") <<
		tr("Log") <<
		tr("Sauvegarde") <<
		tr("Tags artiste") <<
		tr("Tags série") <<
		tr("Tags personnage") <<
		tr("Coloration") <<
		tr("Commandes");
	QMap<QString,int> assoc;
	for (int i = 0; i < texts.count(); i++)
	{ assoc[texts.at(i)] = i; }
	if (previous != NULL)
	{
		int iprevious = assoc[previous->text(0)];
		if (iprevious < ui->container->count())
		{ ui->container->itemAt(iprevious)->widget()->hide(); }
	}
	int icurrent = assoc[current->text(0)];
	if (icurrent < ui->container->count())
	{ ui->container->itemAt(icurrent)->widget()->show(); }
}

void optionsWindow::save()
{
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);

	settings.setValue("dateformat", ui->lineDateFormat->text());
	settings.setValue("updatesrate", ui->spinCheckForUpdates->value());
	settings.setValue("blacklistedtags", ui->lineBlacklist->text());
	settings.setValue("downloadblacklist", ui->checkDownloadBlacklisted->isChecked());

	settings.setValue("limit", ui->spinImagesPerPage->value());
	settings.setValue("columns", ui->spinColumns->value());
	QStringList sources = QStringList() << "xml" << "json" << "regex";
	settings.setValue("source_1", sources.at(ui->comboSource1->currentIndex()));
	settings.setValue("source_2", sources.at(ui->comboSource2->currentIndex()));
	settings.setValue("source_3", sources.at(ui->comboSource3->currentIndex()));
	settings.setValue("loadatstart", ui->checkLoadFirstAtStart->isChecked());

	settings.setValue("Log/show", ui->checkShowLog->isChecked());
	settings.setValue("Log/invert", ui->checkInvertLog->isChecked());

	settings.beginGroup("Save");

		settings.setValue("downloadoriginals", ui->checkDownloadOriginals->isChecked());
		settings.setValue("separator", ui->lineSeparator->text());
		settings.setValue("path", ui->lineFolder->text());
		settings.setValue("filename", ui->lineFilename->text());

		settings.setValue("artist_empty", ui->lineArtistsIfNone->text());
		settings.setValue("artist_useall", ui->checkArtistsKeepAll->isChecked());
		settings.setValue("artist_sep", ui->lineArtistsSeparator->text());
		settings.setValue("artist_value", ui->lineArtistsIfMultiples->text());

		settings.setValue("copyright_empty", ui->lineCopyrightsIfNone->text());
		settings.setValue("copyright_useshorter", ui->checkCopyrightsUseShorter->isChecked());
		settings.setValue("copyright_useall", ui->checkCopyrightsKeepAll->isChecked());
		settings.setValue("copyright_sep", ui->lineCopyrightsSeparator->text());
		settings.setValue("copyright_value", ui->lineCopyrightsIfMultiples->text());

		settings.setValue("character_empty", ui->lineCharactersIfNone->text());
		settings.setValue("character_useall", ui->checkCharactersKeepAll->isChecked());
		settings.setValue("character_sep", ui->lineCharactersSeparator->text());
		settings.setValue("character_value", ui->lineCharactersIfMultiples->text());

	settings.endGroup();

	settings.beginGroup("Coloring");

		settings.setValue("artists", ui->lineColoringArtists->text());
		settings.setValue("copyrights", ui->lineColoringCopyrights->text());
		settings.setValue("characters", ui->lineColoringCharacters->text());

	settings.endGroup();

	settings.beginGroup("Exec");

		settings.setValue("init", ui->lineCommandsInitialisation->text());
		settings.setValue("image", ui->lineCommandsImage->text());
		settings.setValue("tag", ui->lineCommandsTag->text());

	settings.endGroup();

	if (settings.value("language", "English").toString() != ui->comboLanguages->currentText())
	{
		settings.setValue("language", ui->comboLanguages->currentText());
		m_parent->loadLanguage(ui->comboLanguages->currentText());
	}
}
