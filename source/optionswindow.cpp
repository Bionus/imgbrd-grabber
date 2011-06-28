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
	ui->lineBlacklist->setText(settings.value("blacklistedtags").toString());
	ui->checkDownloadBlacklisted->setChecked(settings.value("downloadblacklist", false).toBool());
	ui->checkLoadFirstAtStart->setChecked(settings.value("loadatstart", false).toBool());
	ui->spinHideFavorites->setValue(settings.value("hidefavorites", 20).toInt());
	ui->checkAutodownload->setChecked(settings.value("autodownload", false).toBool());

	ui->spinImagesPerPage->setValue(settings.value("limit", 20).toInt());
	ui->spinColumns->setValue(settings.value("columns", 1).toInt());
	QStringList sources = QStringList() << "xml" << "json" << "regex";
	ui->comboSource1->setCurrentIndex(sources.indexOf(settings.value("source_1", "xml").toString()));
	ui->comboSource2->setCurrentIndex(sources.indexOf(settings.value("source_2", "xml").toString()));
	ui->comboSource3->setCurrentIndex(sources.indexOf(settings.value("source_3", "xml").toString()));

	QStringList types = QStringList() << "text" << "icon" << "both" << "hide";
	ui->comboSources->setCurrentIndex(types.indexOf(settings.value("Sources/Types", "text").toString()));

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
		settings.beginGroup("Colors");
			ui->lineColoringArtists->setText(settings.value("artists", "#aa0000").toString());
			ui->lineColoringCopyrights->setText(settings.value("copyrights", "#aa00aa").toString());
			ui->lineColoringCharacters->setText(settings.value("characters", "#00aa00").toString());
			ui->lineColoringModels->setText(settings.value("models", "#0000ee").toString());
			ui->lineColoringGenerals->setText(settings.value("generals", "#000000").toString());
		settings.endGroup();
		settings.beginGroup("Fonts");
			QFont fontArtists, fontCopyrights, fontCharacters, fontModels, fontGenerals;
			fontArtists.fromString(settings.value("artists").toString());
			fontCopyrights.fromString(settings.value("copyrights").toString());
			fontCharacters.fromString(settings.value("characters").toString());
			fontModels.fromString(settings.value("models").toString());
			fontGenerals.fromString(settings.value("generals").toString());
			ui->lineColoringArtists->setFont(fontArtists);
			ui->lineColoringCopyrights->setFont(fontCopyrights);
			ui->lineColoringCharacters->setFont(fontCharacters);
			ui->lineColoringModels->setFont(fontModels);
			ui->lineColoringGenerals->setFont(fontGenerals);
		settings.endGroup();
	settings.endGroup();

	settings.beginGroup("Margins");
		ui->spinHorizontalMargins->setValue(settings.value("horizontal", 6).toInt());
		ui->spinVerticalMargins->setValue(settings.value("vertical", 6).toInt());
	settings.endGroup();

	settings.beginGroup("Login");
		ui->linePseudo->setText(settings.value("pseudo").toString());
		ui->linePassword->setText(settings.value("password").toString());
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

void optionsWindow::on_lineFilename_textChanged(QString text)
{ ui->filenameValidator->setText(validateFilename(text)); }

void optionsWindow::on_buttonFolder_clicked()
{
	QString folder = QFileDialog::getExistingDirectory(this, tr("Choisir un dossier de sauvegarde"), ui->lineFolder->text());
	if (!folder.isEmpty())
	{ ui->lineFolder->setText(folder); }
}
void optionsWindow::on_buttonCrypt_clicked()
{
	QString password = QInputDialog::getText(this, "Hasher un mot de passe", "Veuillez entrer votre mot de passe.", QLineEdit::Password);
	if (!password.isEmpty())
	{ ui->linePassword->setText(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha1).toHex()); }
}

void optionsWindow::on_lineColoringArtists_textChanged()
{
	if (QColor(ui->lineColoringArtists->text()).isValid())
	{ ui->lineColoringArtists->setStyleSheet("color:"+ui->lineColoringArtists->text()); }
	else
	{ ui->lineColoringArtists->setStyleSheet("color:#000000"); }
}
void optionsWindow::on_lineColoringCopyrights_textChanged()
{
	if (QColor(ui->lineColoringCopyrights->text()).isValid())
	{ ui->lineColoringCopyrights->setStyleSheet("color:"+ui->lineColoringCopyrights->text()); }
	else
	{ ui->lineColoringCopyrights->setStyleSheet("color:#000000"); }
}
void optionsWindow::on_lineColoringCharacters_textChanged()
{
	if (QColor(ui->lineColoringCharacters->text()).isValid())
	{ ui->lineColoringCharacters->setStyleSheet("color:"+ui->lineColoringCharacters->text()); }
	else
	{ ui->lineColoringCharacters->setStyleSheet("color:#000000"); }
}
void optionsWindow::on_lineColoringModels_textChanged()
{
	if (QColor(ui->lineColoringModels->text()).isValid())
	{ ui->lineColoringModels->setStyleSheet("color:"+ui->lineColoringModels->text()); }
	else
	{ ui->lineColoringModels->setStyleSheet("color:#000000"); }
}
void optionsWindow::on_lineColoringGenerals_textChanged()
{
	if (QColor(ui->lineColoringGenerals->text()).isValid())
	{ ui->lineColoringGenerals->setStyleSheet("color:"+ui->lineColoringGenerals->text()); }
	else
	{ ui->lineColoringGenerals->setStyleSheet("color:#000000"); }
}

void optionsWindow::on_buttonColoringArtistsColor_clicked()
{
	QColor color = QColorDialog::getColor(QColor(ui->lineColoringArtists->text()), this, "Grabber - Choisir une couleur");
	if (color.isValid())
	{ ui->lineColoringArtists->setText(color.name()); }
}
void optionsWindow::on_buttonColoringCopyrightsColor_clicked()
{
	QColor color = QColorDialog::getColor(QColor(ui->lineColoringCopyrights->text()), this, "Grabber - Choisir une couleur");
	if (color.isValid())
	{ ui->lineColoringCopyrights->setText(color.name()); }
}
void optionsWindow::on_buttonColoringCharactersColor_clicked()
{
	QColor color = QColorDialog::getColor(QColor(ui->lineColoringCharacters->text()), this, "Grabber - Choisir une couleur");
	if (color.isValid())
	{ ui->lineColoringCharacters->setText(color.name()); }
}
void optionsWindow::on_buttonColoringModelsColor_clicked()
{
	QColor color = QColorDialog::getColor(QColor(ui->lineColoringModels->text()), this, "Grabber - Choisir une couleur");
	if (color.isValid())
	{ ui->lineColoringModels->setText(color.name()); }
}
void optionsWindow::on_buttonColoringGeneralsColor_clicked()
{
	QColor color = QColorDialog::getColor(QColor(ui->lineColoringGenerals->text()), this, "Grabber - Choisir une couleur");
	if (color.isValid())
	{ ui->lineColoringGenerals->setText(color.name()); }
}

void optionsWindow::on_buttonColoringArtistsFont_clicked()
{
	bool ok = false;
	QFont police = QFontDialog::getFont(&ok, ui->lineColoringArtists->font(), this, "Grabber - Choisir une police");
	if (ok)
	{ ui->lineColoringArtists->setFont(police); }
}
void optionsWindow::on_buttonColoringCopyrightsFont_clicked()
{
	bool ok = false;
	QFont police = QFontDialog::getFont(&ok, ui->lineColoringCopyrights->font(), this, "Grabber - Choisir une police");
	if (ok)
	{ ui->lineColoringCopyrights->setFont(police); }
}
void optionsWindow::on_buttonColoringCharactersFont_clicked()
{
	bool ok = false;
	QFont police = QFontDialog::getFont(&ok, ui->lineColoringCharacters->font(), this, "Grabber - Choisir une police");
	if (ok)
	{ ui->lineColoringCharacters->setFont(police); }
}
void optionsWindow::on_buttonColoringModelsFont_clicked()
{
	bool ok = false;
	QFont police = QFontDialog::getFont(&ok, ui->lineColoringModels->font(), this, "Grabber - Choisir une police");
	if (ok)
	{ ui->lineColoringModels->setFont(police); }
}
void optionsWindow::on_buttonColoringGeneralsFont_clicked()
{
	bool ok = false;
	QFont police = QFontDialog::getFont(&ok, ui->lineColoringGenerals->font(), this, "Grabber - Choisir une police");
	if (ok)
	{ ui->lineColoringGenerals->setFont(police); }
}

void optionsWindow::updateContainer(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	QStringList texts = QStringList() <<
		tr("Général", "update") <<
		tr("Sources", "update") <<
		tr("Log", "update") <<
		tr("Sauvegarde", "update") <<
		tr("Tags artiste", "update") <<
		tr("Tags série", "update") <<
		tr("Tags personnage", "update") <<
		tr("Interface", "update") <<
		tr("Coloration", "update") <<
		tr("Marges", "update") <<
		tr("Connexion", "update") <<
		tr("Commandes", "update");
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
	settings.setValue("hidefavorites", ui->spinHideFavorites->value());
	settings.setValue("autodownload", ui->checkAutodownload->isChecked());

	QStringList types = QStringList() << "text" << "icon" << "both" << "hide";
	settings.setValue("Sources/Types", types.at(ui->comboSources->currentIndex()));

	settings.beginGroup("Log");
		settings.setValue("show", ui->checkShowLog->isChecked());
		settings.setValue("invert", ui->checkInvertLog->isChecked());
	settings.endGroup();

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
		settings.beginGroup("Colors");
			settings.setValue("artists", ui->lineColoringArtists->text());
			settings.setValue("copyrights", ui->lineColoringCopyrights->text());
			settings.setValue("characters", ui->lineColoringCharacters->text());
			settings.setValue("models", ui->lineColoringModels->text());
			settings.setValue("generals", ui->lineColoringGenerals->text());
		settings.endGroup();
		settings.beginGroup("Fonts");
			settings.setValue("artists", ui->lineColoringArtists->font().toString());
			settings.setValue("copyrights", ui->lineColoringCopyrights->font().toString());
			settings.setValue("characters", ui->lineColoringCharacters->font().toString());
			settings.setValue("models", ui->lineColoringModels->font().toString());
			settings.setValue("generals", ui->lineColoringGenerals->font().toString());
		settings.endGroup();
	settings.endGroup();

	settings.beginGroup("Margins");
		settings.setValue("horizontal", ui->spinHorizontalMargins->value());
		settings.setValue("vertical", ui->spinVerticalMargins->value());
	settings.endGroup();

	settings.beginGroup("Login");
		settings.setValue("pseudo", ui->linePseudo->text());
		settings.setValue("password", ui->linePassword->text());
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
