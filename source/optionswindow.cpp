#include <QNetworkProxy>
#include <QSettings>
#include <QDir>
#include <QInputDialog>
#include <QFileDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QCryptographicHash>
#include <QSqlDatabase>
#include "optionswindow.h"
#include "ui_optionswindow.h"
#include "customwindow.h"
#include "conditionwindow.h"
#include "filenamewindow.h"
#include "functions.h"
#include "mainwindow.h"



extern mainWindow *_mainwindow;

optionsWindow::optionsWindow(QWidget *parent) : QDialog(parent), ui(new Ui::optionsWindow)
{
	ui->setupUi(this);

	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
	QStringList languages = QDir(qApp->applicationDirPath()+"/languages").entryList(QStringList("*.qm"), QDir::Files);
	for (int i = 0; i < languages.count(); i++)
	{ languages[i].remove(".qm", Qt::CaseInsensitive); }
	ui->comboLanguages->addItems(languages);

	ui->comboLanguages->setCurrentIndex(languages.indexOf(settings.value("language", "English").toString()));
	ui->lineBlacklist->setText(settings.value("blacklistedtags").toString());
	ui->checkDownloadBlacklisted->setChecked(settings.value("downloadblacklist", false).toBool());
	ui->lineWhitelist->setText(settings.value("whitelistedtags").toString());
	ui->lineAdd->setText(settings.value("add").toString());
	QStringList wl = QStringList() << "never" << "image" << "page";
	ui->comboWhitelist->setCurrentIndex(wl.indexOf(settings.value("whitelist_download", "image").toString()));
	ui->lineIgnored->setText(settings.value("ignoredtags").toString());
	QStringList starts = QStringList() << "none" << "loadfirst" << "restore";
	ui->comboStart->setCurrentIndex(starts.indexOf(settings.value("start", "none").toString()));
	ui->spinHideFavorites->setValue(settings.value("hidefavorites", 20).toInt());
	ui->checkAutodownload->setChecked(settings.value("autodownload", false).toBool());
	ui->checkHideBlacklisted->setChecked(settings.value("hideblacklisted", false).toBool());
	ui->checkShowTagWarning->setChecked(settings.value("showtagwarning", true).toBool());
	ui->checkShowWarnings->setChecked(settings.value("showwarnings", true).toBool());
	ui->checkGetUnloadedPages->setChecked(settings.value("getunloadedpages", false).toBool());

	ui->spinImagesPerPage->setValue(settings.value("limit", 20).toInt());
	ui->spinColumns->setValue(settings.value("columns", 1).toInt());
	QStringList sources = QStringList() << "xml" << "json" << "regex" << "rss";
	ui->comboSource1->setCurrentIndex(sources.indexOf(settings.value("source_1", "xml").toString()));
	ui->comboSource2->setCurrentIndex(sources.indexOf(settings.value("source_2", "json").toString()));
	ui->comboSource3->setCurrentIndex(sources.indexOf(settings.value("source_3", "regex").toString()));
	ui->comboSource4->setCurrentIndex(sources.indexOf(settings.value("source_4", "rss").toString()));
	ui->spinAutoTagAdd->setValue(settings.value("tagsautoadd", 10).toInt());

	QStringList positions = QStringList() << "top" << "left" << "auto";
	ui->comboTagsposition->setCurrentIndex(positions.indexOf(settings.value("tagsposition", "top").toString()));

	QMap<QString,QString> filenames = getFilenames();
	m_filenamesConditions = QList<QLineEdit*>();
	m_filenamesFilenames = QList<QLineEdit*>();
	for (int i = 0; i < filenames.size(); i++)
	{
		QLineEdit *leCondition = new QLineEdit(filenames.keys().at(i));
		QLineEdit *leFilename = new QLineEdit(filenames.values().at(i));
		m_filenamesConditions.append(leCondition);
		m_filenamesFilenames.append(leFilename);
		ui->layoutFilenames->insertRow(i, leCondition, leFilename);
	}
	QStringList types = QStringList() << "text" << "icon" << "both" << "hide";
	ui->comboSources->setCurrentIndex(types.indexOf(settings.value("Sources/Types", "icon").toString()));
	int i = settings.value("Sources/Letters", 3).toInt();
	ui->comboSourcesLetters->setCurrentIndex((i < 0)+(i < -1));
	ui->spinSourcesLetters->setValue(i < 0 ? 3 : i);

	QStringList ftypes = QStringList() << "ind" << "in" << "id" << "nd" << "i" << "n" << "d";
	ui->comboFavoritesDisplay->setCurrentIndex(ftypes.indexOf(settings.value("favorites_display", "ind").toString()));

	ui->checkShowLog->setChecked(settings.value("Log/show", true).toBool());
	ui->checkInvertLog->setChecked(settings.value("Log/invert", false).toBool());

	ui->checkResizeInsteadOfCropping->setChecked(settings.value("resizeInsteadOfCropping", true).toBool());
	ui->checkAutocompletion->setChecked(settings.value("autocompletion", true).toBool());
	ui->checkUseregexfortags->setChecked(settings.value("useregexfortags", true).toBool());

	ui->checkTextfileActivate->setChecked(settings.value("Textfile/activate", false).toBool());
	ui->textEditTextfileContent->setEnabled(settings.value("Textfile/activate", false).toBool());
	ui->textEditTextfileContent->setPlainText(settings.value("Textfile/content", "%all%").toString());

	ui->checkSaveLogEnable->setChecked(settings.value("SaveLog/activate", false).toBool());
	ui->lineSaveLogFile->setEnabled(settings.value("SaveLog/activate", false).toBool());
	ui->lineSaveLogFile->setText(settings.value("SaveLog/file", "").toString());
	ui->lineSaveLogFormat->setEnabled(settings.value("SaveLog/activate", false).toBool());
	ui->lineSaveLogFormat->setText(settings.value("SaveLog/format", "%website% - %md5% - %all%").toString());

	ui->comboBatchEnd->setCurrentIndex(settings.value("Batch/end", 0).toInt());
	settings.beginGroup("Save");
		ui->spinAutomaticRetries->setValue(settings.value("automaticretries", 0).toInt());
		ui->checkDownloadOriginals->setChecked(settings.value("downloadoriginals", true).toBool());
		ui->checkSampleFallback->setChecked(settings.value("samplefallback", true).toBool());
		ui->checkReplaceBlanks->setChecked(settings.value("replaceblanks", false).toBool());
		ui->lineFolder->setText(settings.value("path_real").toString());
		ui->lineFolderFavorites->setText(settings.value("path_favorites").toString());
		QStringList opts = QStringList() << "save" << "copy" << "move" << "ignore";
		ui->comboMd5Duplicates->setCurrentIndex(opts.indexOf(settings.value("md5Duplicates", "save").toString()));
		ui->lineFilename->setText(settings.value("filename_real").toString());
		ui->lineFavorites->setText(settings.value("filename_favorites").toString());
		ui->lineSeparator->setText(settings.value("separator", " ").toString());
		ui->lineArtistsIfNone->setText(settings.value("artist_empty", "anonymous").toString());
		ui->spinArtistsMoreThanN->setValue(settings.value("artist_multiple_limit", 1).toInt());
		ui->spinArtistsKeepN->setValue(settings.value("artist_multiple_keepN", 1).toInt());
		ui->spinArtistsKeepNThenAdd->setValue(settings.value("artist_multiple_keepNThenAdd_keep", 1).toInt());
		ui->lineArtistsKeepNThenAdd->setText(settings.value("artist_multiple_keepNThenAdd_add", " (+ %count%)").toString());
		ui->lineArtistsSeparator->setText(settings.value("artist_sep", "+").toString());
		ui->lineArtistsReplaceAll->setText(settings.value("artist_value", "multiple artists").toString());
		QString artistMultiple = settings.value("artist_multiple", "replaceAll").toString();
		if		(artistMultiple == "keepAll")		{ ui->radioArtistsKeepAll->setChecked(true);		}
		else if	(artistMultiple == "keepN")			{ ui->radioArtistsKeepN->setChecked(true);			}
		else if	(artistMultiple == "keepNThenAdd")	{ ui->radioArtistsKeepNThenAdd->setChecked(true);	}
		else if	(artistMultiple == "replaceAll")	{ ui->radioArtistsReplaceAll->setChecked(true);		}
		ui->lineCopyrightsIfNone->setText(settings.value("copyright_empty", "misc").toString());
		ui->checkCopyrightsUseShorter->setChecked(settings.value("copyright_useshorter", true).toBool());
		ui->spinCopyrightsMoreThanN->setValue(settings.value("copyright_multiple_limit", 1).toInt());
		ui->spinCopyrightsKeepN->setValue(settings.value("copyright_multiple_keepN", 1).toInt());
		ui->spinCopyrightsKeepNThenAdd->setValue(settings.value("copyright_multiple_keepNThenAdd_keep", 1).toInt());
		ui->lineCopyrightsKeepNThenAdd->setText(settings.value("copyright_multiple_keepNThenAdd_add", " (+ %count%)").toString());
		ui->lineCopyrightsSeparator->setText(settings.value("copyright_sep", "+").toString());
		ui->lineCopyrightsReplaceAll->setText(settings.value("copyright_value", "crossover").toString());
		QString copyrightMultiple = settings.value("copyright_multiple", "replaceAll").toString();
		if		(copyrightMultiple == "keepAll")		{ ui->radioCopyrightsKeepAll->setChecked(true);			}
		else if	(copyrightMultiple == "keepN")			{ ui->radioCopyrightsKeepN->setChecked(true);			}
		else if	(copyrightMultiple == "keepNThenAdd")	{ ui->radioCopyrightsKeepNThenAdd->setChecked(true);	}
		else if	(copyrightMultiple == "replaceAll")		{ ui->radioCopyrightsReplaceAll->setChecked(true);		}
		ui->lineCharactersIfNone->setText(settings.value("character_empty", "unknown").toString());
		ui->spinCharactersMoreThanN->setValue(settings.value("character_multiple_limit", 1).toInt());
		ui->spinCharactersKeepN->setValue(settings.value("character_multiple_keepN", 1).toInt());
		ui->spinCharactersKeepNThenAdd->setValue(settings.value("character_multiple_keepNThenAdd_keep", 1).toInt());
		ui->lineCharactersKeepNThenAdd->setText(settings.value("character_multiple_keepNThenAdd_add", " (+ %count%)").toString());
		ui->lineCharactersSeparator->setText(settings.value("character_sep", "+").toString());
		ui->lineCharactersReplaceAll->setText(settings.value("character_value", "group").toString());
		QString characterMultiple = settings.value("character_multiple", "replaceAll").toString();
		if		(characterMultiple == "keepAll")		{ ui->radioCharactersKeepAll->setChecked(true);			}
		else if	(characterMultiple == "keepN")			{ ui->radioCharactersKeepN->setChecked(true);			}
		else if	(characterMultiple == "keepNThenAdd")	{ ui->radioCharactersKeepNThenAdd->setChecked(true);	}
		else if	(characterMultiple == "replaceAll")		{ ui->radioCharactersReplaceAll->setChecked(true);		}
		ui->spinLimit->setValue(settings.value("limit", 0).toInt());
		ui->spinSimultaneous->setValue(settings.value("simultaneous", 1).toInt());
		QMap<QString,QStringList> customs = getCustoms();
		settings.beginGroup("Customs");
			m_customNames = QList<QLineEdit*>();
			m_customTags = QList<QLineEdit*>();
			for (int i = 0; i < customs.size(); i++)
			{
				QLineEdit *leName = new QLineEdit(customs.keys().at(i));
				QLineEdit *leTags = new QLineEdit(customs.values().at(i).join(" "));
				m_customNames.append(leName);
				m_customTags.append(leTags);
				ui->layoutCustom->insertRow(i, leName, leTags);
			}
		settings.endGroup();
	settings.endGroup();

	settings.beginGroup("Coloring");
		settings.beginGroup("Colors");
			ui->lineColoringArtists->setText(settings.value("artists", "#aa0000").toString());
			ui->lineColoringCircles->setText(settings.value("circles", "#55bbff").toString());
			ui->lineColoringCopyrights->setText(settings.value("copyrights", "#aa00aa").toString());
			ui->lineColoringCharacters->setText(settings.value("characters", "#00aa00").toString());
			ui->lineColoringModels->setText(settings.value("models", "#0000ee").toString());
			ui->lineColoringGenerals->setText(settings.value("generals", "#000000").toString());
			ui->lineColoringFavorites->setText(settings.value("favorites", "#ffc0cb").toString());
			ui->lineColoringBlacklisteds->setText(settings.value("blacklisteds", "#000000").toString());
			ui->lineColoringIgnoreds->setText(settings.value("ignoreds", "#999999").toString());
		settings.endGroup();
		settings.beginGroup("Fonts");
			QFont fontArtists, fontCircles, fontCopyrights, fontCharacters, fontModels, fontGenerals, fontFavorites, fontBlacklisteds, fontIgnoreds;
			fontArtists.fromString(settings.value("artists").toString());
			fontCircles.fromString(settings.value("circles").toString());
			fontCopyrights.fromString(settings.value("copyrights").toString());
			fontCharacters.fromString(settings.value("characters").toString());
			fontModels.fromString(settings.value("models").toString());
			fontGenerals.fromString(settings.value("generals").toString());
			fontFavorites.fromString(settings.value("favorites").toString());
			fontBlacklisteds.fromString(settings.value("blacklisteds").toString());
			fontIgnoreds.fromString(settings.value("ignoreds").toString());
			ui->lineColoringArtists->setFont(fontArtists);
			ui->lineColoringCircles->setFont(fontCircles);
			ui->lineColoringCopyrights->setFont(fontCopyrights);
			ui->lineColoringCharacters->setFont(fontCharacters);
			ui->lineColoringModels->setFont(fontModels);
			ui->lineColoringGenerals->setFont(fontGenerals);
			ui->lineColoringFavorites->setFont(fontFavorites);
			ui->lineColoringBlacklisteds->setFont(fontBlacklisteds);
			ui->lineColoringIgnoreds->setFont(fontIgnoreds);
		settings.endGroup();
	settings.endGroup();

	settings.beginGroup("Margins");
		ui->spinMainMargins->setValue(settings.value("main", 10).toInt());
		ui->spinHorizontalMargins->setValue(settings.value("horizontal", 6).toInt());
		ui->spinVerticalMargins->setValue(settings.value("vertical", 6).toInt());
	settings.endGroup();
	ui->spinServerBorders->setValue(settings.value("serverBorder", 0).toInt());
	ui->lineBorderColor->setText(settings.value("serverBorderColor", "#000000").toString());
	ui->spinBorders->setValue(settings.value("borders", 3).toInt());

	settings.beginGroup("Proxy");
		ui->checkProxyUse->setChecked(settings.value("use", false).toBool());
		QStringList ptypes = QStringList() << "http" << "socks5";
		ui->comboProxyType->setCurrentIndex(ptypes.indexOf(settings.value("type", "http").toString()));
		ui->widgetProxy->setEnabled(settings.value("use", false).toBool());
		ui->lineProxyHostName->setText(settings.value("hostName").toString());
		ui->spinProxyPort->setValue(settings.value("port").toInt());
	settings.endGroup();

	settings.beginGroup("Exec");
		ui->lineCommandsImage->setText(settings.value("image").toString());
		ui->lineCommandsTag->setText(settings.value("tag").toString());
		settings.beginGroup("SQL");
			ui->comboCommandsSqlDriver->addItems(QSqlDatabase::drivers());
			ui->comboCommandsSqlDriver->setCurrentIndex(QSqlDatabase::drivers().indexOf(settings.value("driver", "QMYSQL").toString()));
			ui->lineCommandsSqlHost->setText(settings.value("host").toString());
			ui->lineCommandsSqlUser->setText(settings.value("user").toString());
			ui->lineCommandsSqlPassword->setText(settings.value("password").toString());
			ui->lineCommandsSqlDatabase->setText(settings.value("database").toString());
			ui->lineCommandsSqlBefore->setText(settings.value("before").toString());
			ui->lineCommandsSqlImage->setText(settings.value("image").toString());
			ui->lineCommandsSqlTag->setText(settings.value("tag").toString());
			ui->lineCommandsSqlAfter->setText(settings.value("after").toString());
		settings.endGroup();
	settings.endGroup();
	connect(this, SIGNAL(accepted()), this, SLOT(save()));
}

optionsWindow::~optionsWindow()
{
	delete ui;
}

void optionsWindow::on_comboSourcesLetters_currentIndexChanged(int i)
{ ui->spinSourcesLetters->setDisabled(i > 0); }

void optionsWindow::on_buttonFolder_clicked()
{
	QString folder = QFileDialog::getExistingDirectory(this, tr("Choisir un dossier de sauvegarde"), ui->lineFolder->text());
	if (!folder.isEmpty())
	{ ui->lineFolder->setText(folder); }
}
void optionsWindow::on_buttonFolderFavorites_clicked()
{
	QString folder = QFileDialog::getExistingDirectory(this, tr("Choisir un dossier de sauvegarde pour les favoris"), ui->lineFolderFavorites->text());
	if (!folder.isEmpty())
	{ ui->lineFolderFavorites->setText(folder); }
}

void optionsWindow::on_buttonFilenamePlus_clicked()
{
	FilenameWindow *fw = new FilenameWindow(ui->lineFilename->text(), this);
	connect(fw, SIGNAL(validated(QString)), ui->lineFilename, SLOT(setText(QString)));
	fw->show();
}
void optionsWindow::on_buttonFavoritesPlus_clicked()
{
	FilenameWindow *fw = new FilenameWindow(ui->lineFavorites->text(), this);
	connect(fw, SIGNAL(validated(QString)), ui->lineFavorites, SLOT(setText(QString)));
	fw->show();
}

void optionsWindow::on_buttonCustom_clicked()
{
	customWindow *cw = new customWindow(this);
	connect(cw, SIGNAL(validated(QString, QString)), this, SLOT(addCustom(QString, QString)));
	cw->show();
}
void optionsWindow::addCustom(QString name, QString tags)
{
	QLineEdit *leName = new QLineEdit(name);
	QLineEdit *leTags = new QLineEdit(tags);
	ui->layoutCustom->insertRow(m_customNames.size(), leName, leTags);
	m_customNames.append(leName);
	m_customTags.append(leTags);
}
void optionsWindow::on_buttonFilenames_clicked()
{
	conditionWindow *cw = new conditionWindow();
	connect(cw, SIGNAL(validated(QString, QString)), this, SLOT(addFilename(QString, QString)));
	cw->show();
}
void optionsWindow::addFilename(QString condition, QString filename)
{
	QLineEdit *leCondition = new QLineEdit(condition);
	QLineEdit *leFilename = new QLineEdit(filename);
	ui->layoutFilenames->insertRow(m_filenamesConditions.size(), leCondition, leFilename);
	m_filenamesConditions.append(leCondition);
	m_filenamesFilenames.append(leFilename);
}

void optionsWindow::on_lineColoringArtists_textChanged()
{
	if (QColor(ui->lineColoringArtists->text()).isValid())
	{ ui->lineColoringArtists->setStyleSheet("color:"+ui->lineColoringArtists->text()); }
	else
	{ ui->lineColoringArtists->setStyleSheet("color:#000000"); }
}
void optionsWindow::on_lineColoringCircles_textChanged()
{
	if (QColor(ui->lineColoringCircles->text()).isValid())
	{ ui->lineColoringCircles->setStyleSheet("color:"+ui->lineColoringCircles->text()); }
	else
	{ ui->lineColoringCircles->setStyleSheet("color:#000000"); }
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
void optionsWindow::on_lineColoringFavorites_textChanged()
{
	if (QColor(ui->lineColoringFavorites->text()).isValid())
	{ ui->lineColoringFavorites->setStyleSheet("color:"+ui->lineColoringFavorites->text()); }
	else
	{ ui->lineColoringFavorites->setStyleSheet("color:#000000"); }
}
void optionsWindow::on_lineColoringBlacklisteds_textChanged()
{
	if (QColor(ui->lineColoringBlacklisteds->text()).isValid())
	{ ui->lineColoringBlacklisteds->setStyleSheet("color:"+ui->lineColoringBlacklisteds->text()); }
	else
	{ ui->lineColoringBlacklisteds->setStyleSheet("color:#000000"); }
}
void optionsWindow::on_lineColoringIgnoreds_textChanged()
{
	if (QColor(ui->lineColoringIgnoreds->text()).isValid())
	{ ui->lineColoringIgnoreds->setStyleSheet("color:"+ui->lineColoringIgnoreds->text()); }
	else
	{ ui->lineColoringIgnoreds->setStyleSheet("color:#000000"); }
}

void optionsWindow::on_buttonColoringArtistsColor_clicked()
{
	QColor color = QColorDialog::getColor(QColor(ui->lineColoringArtists->text()), this, tr("Choisir une couleur"));
	if (color.isValid())
	{ ui->lineColoringArtists->setText(color.name()); }
}
void optionsWindow::on_buttonColoringCirclesColor_clicked()
{
	QColor color = QColorDialog::getColor(QColor(ui->lineColoringCircles->text()), this, tr("Choisir une couleur"));
	if (color.isValid())
	{ ui->lineColoringCircles->setText(color.name()); }
}
void optionsWindow::on_buttonColoringCopyrightsColor_clicked()
{
	QColor color = QColorDialog::getColor(QColor(ui->lineColoringCopyrights->text()), this, tr("Choisir une couleur"));
	if (color.isValid())
	{ ui->lineColoringCopyrights->setText(color.name()); }
}
void optionsWindow::on_buttonColoringCharactersColor_clicked()
{
	QColor color = QColorDialog::getColor(QColor(ui->lineColoringCharacters->text()), this, tr("Choisir une couleur"));
	if (color.isValid())
	{ ui->lineColoringCharacters->setText(color.name()); }
}
void optionsWindow::on_buttonColoringModelsColor_clicked()
{
	QColor color = QColorDialog::getColor(QColor(ui->lineColoringModels->text()), this, tr("Choisir une couleur"));
	if (color.isValid())
	{ ui->lineColoringModels->setText(color.name()); }
}
void optionsWindow::on_buttonColoringGeneralsColor_clicked()
{
	QColor color = QColorDialog::getColor(QColor(ui->lineColoringGenerals->text()), this, tr("Choisir une couleur"));
	if (color.isValid())
	{ ui->lineColoringGenerals->setText(color.name()); }
}
void optionsWindow::on_buttonColoringFavoritesColor_clicked()
{
	QColor color = QColorDialog::getColor(QColor(ui->lineColoringFavorites->text()), this, tr("Choisir une couleur"));
	if (color.isValid())
	{ ui->lineColoringFavorites->setText(color.name()); }
}
void optionsWindow::on_buttonColoringBlacklistedsColor_clicked()
{
	QColor color = QColorDialog::getColor(QColor(ui->lineColoringBlacklisteds->text()), this, tr("Choisir une couleur"));
	if (color.isValid())
	{ ui->lineColoringBlacklisteds->setText(color.name()); }
}
void optionsWindow::on_buttonColoringIgnoredsColor_clicked()
{
	QColor color = QColorDialog::getColor(QColor(ui->lineColoringIgnoreds->text()), this, tr("Choisir une couleur"));
	if (color.isValid())
	{ ui->lineColoringIgnoreds->setText(color.name()); }
}

void optionsWindow::on_buttonColoringArtistsFont_clicked()
{
	bool ok = false;
	QFont police = QFontDialog::getFont(&ok, ui->lineColoringArtists->font(), this, tr("Choisir une police"));
	if (ok)
	{ ui->lineColoringArtists->setFont(police); }
}
void optionsWindow::on_buttonColoringCirclesFont_clicked()
{
	bool ok = false;
	QFont police = QFontDialog::getFont(&ok, ui->lineColoringCircles->font(), this, tr("Choisir une police"));
	if (ok)
	{ ui->lineColoringCircles->setFont(police); }
}
void optionsWindow::on_buttonColoringCopyrightsFont_clicked()
{
	bool ok = false;
	QFont police = QFontDialog::getFont(&ok, ui->lineColoringCopyrights->font(), this, tr("Choisir une police"));
	if (ok)
	{ ui->lineColoringCopyrights->setFont(police); }
}
void optionsWindow::on_buttonColoringCharactersFont_clicked()
{
	bool ok = false;
	QFont police = QFontDialog::getFont(&ok, ui->lineColoringCharacters->font(), this, tr("Choisir une police"));
	if (ok)
	{ ui->lineColoringCharacters->setFont(police); }
}
void optionsWindow::on_buttonColoringModelsFont_clicked()
{
	bool ok = false;
	QFont police = QFontDialog::getFont(&ok, ui->lineColoringModels->font(), this, tr("Choisir une police"));
	if (ok)
	{ ui->lineColoringModels->setFont(police); }
}
void optionsWindow::on_buttonColoringGeneralsFont_clicked()
{
	bool ok = false;
	QFont police = QFontDialog::getFont(&ok, ui->lineColoringGenerals->font(), this, tr("Choisir une police"));
	if (ok)
	{ ui->lineColoringGenerals->setFont(police); }
}
void optionsWindow::on_buttonColoringFavoritesFont_clicked()
{
	bool ok = false;
	QFont police = QFontDialog::getFont(&ok, ui->lineColoringFavorites->font(), this, tr("Choisir une police"));
	if (ok)
	{ ui->lineColoringFavorites->setFont(police); }
}
void optionsWindow::on_buttonColoringBlacklistedsFont_clicked()
{
	bool ok = false;
	QFont police = QFontDialog::getFont(&ok, ui->lineColoringBlacklisteds->font(), this, tr("Choisir une police"));
	if (ok)
	{ ui->lineColoringBlacklisteds->setFont(police); }
}
void optionsWindow::on_buttonColoringIgnoredsFont_clicked()
{
	bool ok = false;
	QFont police = QFontDialog::getFont(&ok, ui->lineColoringIgnoreds->font(), this, tr("Choisir une police"));
	if (ok)
	{ ui->lineColoringIgnoreds->setFont(police); }
}

void optionsWindow::on_lineBorderColor_textChanged()
{
	if (QColor(ui->lineBorderColor->text()).isValid())
	{ ui->lineBorderColor->setStyleSheet("color:"+ui->lineBorderColor->text()); }
	else
	{ ui->lineBorderColor->setStyleSheet("color:#000000"); }
}
void optionsWindow::on_buttonBorderColor_clicked()
{
	QColor color = QColorDialog::getColor(QColor(ui->lineBorderColor->text()), this, tr("Choisir une couleur"));
	if (color.isValid())
	{ ui->lineBorderColor->setText(color.name()); }
}

void optionsWindow::updateContainer(QTreeWidgetItem *current, QTreeWidgetItem *)
{ setCategory(current->text(0)); }
void optionsWindow::setCategory(QString value)
{
	QStringList texts = QStringList() <<
		tr("Général", "update") <<
		tr("Sources", "update") <<
		tr("Log", "update") <<
		tr("Sauvegarde", "update") <<
		tr("Nom de fichier", "update") <<
		tr("Fichier texte séparé", "update") <<
		tr("Log séparé", "update") <<
		tr("Noms multiples", "update") <<
		tr("Tags artiste", "update") <<
		tr("Tags série", "update") <<
		tr("Tags personnage", "update") <<
		tr("Symbole personnalisé", "update") <<
		tr("Interface", "update") <<
		tr("Coloration", "update") <<
		tr("Marges et bordures", "update") <<
		tr("Proxy", "update") <<
		tr("Commandes", "update") <<
		tr("Base de données", "update");
	ui->stackedWidget->setCurrentIndex(texts.indexOf(value));
}

void optionsWindow::save()
{
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);

	settings.setValue("blacklistedtags", ui->lineBlacklist->text());
	settings.setValue("downloadblacklist", ui->checkDownloadBlacklisted->isChecked());
	settings.setValue("whitelistedtags", ui->lineWhitelist->text());
	settings.setValue("ignoredtags", ui->lineIgnored->text());
	settings.setValue("add", ui->lineAdd->text());
	QStringList wl = QStringList() << "never" << "image" << "page";
	settings.setValue("whitelist_download", wl.at(ui->comboWhitelist->currentIndex()));

	settings.setValue("limit", ui->spinImagesPerPage->value());
	settings.setValue("columns", ui->spinColumns->value());
	QStringList sources = QStringList() << "xml" << "json" << "regex" << "rss";
	settings.setValue("source_1", sources.at(ui->comboSource1->currentIndex()));
	settings.setValue("source_2", sources.at(ui->comboSource2->currentIndex()));
	settings.setValue("source_3", sources.at(ui->comboSource3->currentIndex()));
	settings.setValue("source_4", sources.at(ui->comboSource4->currentIndex()));
	settings.setValue("tagsautoadd", ui->spinAutoTagAdd->value());
	QStringList starts = QStringList() << "none" << "loadfirst" << "restore";
	settings.setValue("start", starts.at(ui->comboStart->currentIndex()));
	settings.setValue("hidefavorites", ui->spinHideFavorites->value());
	settings.setValue("autodownload", ui->checkAutodownload->isChecked());
	QStringList positions = QStringList() << "top" << "left" << "auto";
	settings.setValue("tagsposition", positions.at(ui->comboTagsposition->currentIndex()));
	settings.setValue("hideblacklisted", ui->checkHideBlacklisted->isChecked());
	settings.setValue("showtagwarning", ui->checkShowTagWarning->isChecked());
	settings.setValue("showwarnings", ui->checkShowWarnings->isChecked());
	settings.setValue("getunloadedpages", ui->checkGetUnloadedPages->isChecked());

	settings.beginGroup("Filenames");
		settings.remove("");
		for (int i = 0; i < m_filenamesConditions.size(); i++)
		{ settings.setValue(m_filenamesConditions.at(i)->text(), m_filenamesFilenames.at(i)->text()); }
	settings.endGroup();

	QStringList types = QStringList() << "text" << "icon" << "both" << "hide";
	settings.setValue("Sources/Types", types.at(ui->comboSources->currentIndex()));
	int i = ui->comboSourcesLetters->currentIndex();
	settings.setValue("Sources/Letters", (i == 0 ? ui->spinSourcesLetters->value() : -i));

	QStringList ftypes = QStringList() << "ind" << "in" << "id" << "nd" << "i" << "n" << "d";
	if (settings.value("favorites_display", "ind").toString() != ftypes.at(ui->comboFavoritesDisplay->currentIndex()))
	{
		settings.setValue("favorites_display", ftypes.at(ui->comboFavoritesDisplay->currentIndex()));
		_mainwindow->updateFavorites(false);
	}

	settings.beginGroup("Log");
		settings.setValue("show", ui->checkShowLog->isChecked());
		settings.setValue("invert", ui->checkInvertLog->isChecked());
	settings.endGroup();

	settings.setValue("resizeInsteadOfCropping", ui->checkResizeInsteadOfCropping->isChecked());
	settings.setValue("autocompletion", ui->checkAutocompletion->isChecked());
	settings.setValue("useregexfortags", ui->checkUseregexfortags->isChecked());

	settings.beginGroup("Textfile");
		settings.setValue("activate", ui->checkTextfileActivate->isChecked());
		settings.setValue("content", ui->textEditTextfileContent->toPlainText());
	settings.endGroup();

	settings.beginGroup("SaveLog");
		settings.setValue("activate", ui->checkSaveLogEnable->isChecked());
		settings.setValue("file", ui->lineSaveLogFile->text());
		settings.setValue("format", ui->lineSaveLogFormat->text());
	settings.endGroup();

	settings.setValue("Batch/end", ui->comboBatchEnd->currentIndex());
	settings.beginGroup("Save");
		settings.setValue("automaticretries", ui->spinAutomaticRetries->value());
		settings.setValue("downloadoriginals", ui->checkDownloadOriginals->isChecked());
		settings.setValue("samplefallback", ui->checkSampleFallback->isChecked());
		settings.setValue("replaceblanks", ui->checkReplaceBlanks->isChecked());
		settings.setValue("separator", ui->lineSeparator->text());
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
		settings.setValue("path_favorites", ui->lineFolderFavorites->text());
		pth = QDir(ui->lineFolderFavorites->text());
		if (!pth.exists())
		{
			QString op = "";
			while (!pth.exists() && pth.path() != op)
			{
				op = pth.path();
				pth.setPath(pth.path().remove(QRegExp("/([^/]+)$")));
			}
			if (pth.path() == op)
			{ error(this, tr("Une erreur est survenue lors de la création du dossier de sauvegarde des favoris.")); }
			else
			{ pth.mkpath(ui->lineFolderFavorites->text()); }
		}
		QStringList opts = QStringList() << "save" << "copy" << "move" << "ignore";
		settings.setValue("md5Duplicates", opts.at(ui->comboMd5Duplicates->currentIndex()));
		settings.setValue("filename", ui->lineFilename->text());
		settings.setValue("filename_real", ui->lineFilename->text());
		settings.setValue("filename_favorites", ui->lineFavorites->text());
		settings.setValue("artist_empty", ui->lineArtistsIfNone->text());
		settings.setValue("artist_useall", ui->radioArtistsKeepAll->isChecked());
		QString artistMultiple;
		if		(ui->radioArtistsKeepAll->isChecked())		{ artistMultiple = "keepAll";		}
		else if	(ui->radioArtistsKeepN->isChecked())		{ artistMultiple = "keepN";			}
		else if	(ui->radioArtistsKeepNThenAdd->isChecked())	{ artistMultiple = "keepNThenAdd";	}
		else if	(ui->radioArtistsReplaceAll->isChecked())	{ artistMultiple = "replaceAll";	}
		settings.setValue("artist_multiple", artistMultiple);
		settings.setValue("artist_multiple_limit", ui->spinArtistsMoreThanN->value());
		settings.setValue("artist_multiple_keepN", ui->spinArtistsKeepN->value());
		settings.setValue("artist_multiple_keepNThenAdd_keep", ui->spinArtistsKeepNThenAdd->value());
		settings.setValue("artist_multiple_keepNThenAdd_add", ui->lineArtistsKeepNThenAdd->text());
		settings.setValue("artist_sep", ui->lineArtistsSeparator->text());
		settings.setValue("artist_value", ui->lineArtistsReplaceAll->text());
		settings.setValue("copyright_empty", ui->lineCopyrightsIfNone->text());
		settings.setValue("copyright_useshorter", ui->checkCopyrightsUseShorter->isChecked());
		QString copyrightMultiple;
		if		(ui->radioCopyrightsKeepAll->isChecked())		{ copyrightMultiple = "keepAll";		}
		else if	(ui->radioCopyrightsKeepN->isChecked())			{ copyrightMultiple = "keepN";			}
		else if	(ui->radioCopyrightsKeepNThenAdd->isChecked())	{ copyrightMultiple = "keepNThenAdd";	}
		else if	(ui->radioCopyrightsReplaceAll->isChecked())	{ copyrightMultiple = "replaceAll";		}
		settings.setValue("copyright_multiple", copyrightMultiple);
		settings.setValue("copyright_multiple_limit", ui->spinCopyrightsMoreThanN->value());
		settings.setValue("copyright_multiple_keepN", ui->spinCopyrightsKeepN->value());
		settings.setValue("copyright_multiple_keepNThenAdd_keep", ui->spinCopyrightsKeepNThenAdd->value());
		settings.setValue("copyright_multiple_keepNThenAdd_add", ui->lineCopyrightsKeepNThenAdd->text());
		settings.setValue("copyright_sep", ui->lineCopyrightsSeparator->text());
		settings.setValue("copyright_value", ui->lineCopyrightsReplaceAll->text());
		settings.setValue("character_empty", ui->lineCharactersIfNone->text());
		QString characterMultiple;
		if		(ui->radioCharactersKeepAll->isChecked())		{ characterMultiple = "keepAll";		}
		else if	(ui->radioCharactersKeepN->isChecked())			{ characterMultiple = "keepN";			}
		else if	(ui->radioCharactersKeepNThenAdd->isChecked())	{ characterMultiple = "keepNThenAdd";	}
		else if	(ui->radioCharactersReplaceAll->isChecked())	{ characterMultiple = "replaceAll";		}
		settings.setValue("character_multiple", characterMultiple);
		settings.setValue("character_multiple_limit", ui->spinCharactersMoreThanN->value());
		settings.setValue("character_multiple_keepN", ui->spinCharactersKeepN->value());
		settings.setValue("character_multiple_keepNThenAdd_keep", ui->spinCharactersKeepNThenAdd->value());
		settings.setValue("character_multiple_keepNThenAdd_add", ui->lineCharactersKeepNThenAdd->text());
		settings.setValue("character_sep", ui->lineCharactersSeparator->text());
		settings.setValue("character_value", ui->lineCharactersReplaceAll->text());
		settings.setValue("limit", ui->spinLimit->value());
		settings.setValue("simultaneous", ui->spinSimultaneous->value());
		settings.beginGroup("Customs");
			settings.remove("");
			for (int i = 0; i < m_customNames.size(); i++)
			{ settings.setValue(m_customNames.at(i)->text(), m_customTags.at(i)->text()); }
		settings.endGroup();
	settings.endGroup();

	settings.beginGroup("Coloring");
		settings.beginGroup("Colors");
			settings.setValue("artists", ui->lineColoringArtists->text());
			settings.setValue("circles", ui->lineColoringCircles->text());
			settings.setValue("copyrights", ui->lineColoringCopyrights->text());
			settings.setValue("characters", ui->lineColoringCharacters->text());
			settings.setValue("models", ui->lineColoringModels->text());
			settings.setValue("generals", ui->lineColoringGenerals->text());
			settings.setValue("favorites", ui->lineColoringFavorites->text());
			settings.setValue("blacklisteds", ui->lineColoringBlacklisteds->text());
			settings.setValue("ignoreds", ui->lineColoringIgnoreds->text());
		settings.endGroup();
		settings.beginGroup("Fonts");
			settings.setValue("artists", ui->lineColoringArtists->font().toString());
			settings.setValue("circles", ui->lineColoringCircles->font().toString());
			settings.setValue("copyrights", ui->lineColoringCopyrights->font().toString());
			settings.setValue("characters", ui->lineColoringCharacters->font().toString());
			settings.setValue("models", ui->lineColoringModels->font().toString());
			settings.setValue("generals", ui->lineColoringGenerals->font().toString());
			settings.setValue("favorites", ui->lineColoringFavorites->font().toString());
			settings.setValue("blacklisteds", ui->lineColoringBlacklisteds->font().toString());
			settings.setValue("ignoreds", ui->lineColoringIgnoreds->font().toString());
		settings.endGroup();
	settings.endGroup();

	settings.beginGroup("Margins");
		settings.setValue("main", ui->spinMainMargins->value());
		settings.setValue("horizontal", ui->spinHorizontalMargins->value());
		settings.setValue("vertical", ui->spinVerticalMargins->value());
	settings.endGroup();
	settings.setValue("serverBorder", ui->spinServerBorders->value());
	settings.setValue("serverBorderColor", ui->lineBorderColor->text());
	settings.setValue("borders", ui->spinBorders->value());

	settings.beginGroup("Proxy");
		settings.setValue("use", ui->checkProxyUse->isChecked());
		QStringList ptypes = QStringList() << "http" << "socks5";
		settings.setValue("type", ptypes.at(ui->comboProxyType->currentIndex()));
		settings.setValue("hostName", ui->lineProxyHostName->text());
		settings.setValue("port", ui->spinProxyPort->value());
	settings.endGroup();

	settings.beginGroup("Exec");
		settings.setValue("image", ui->lineCommandsImage->text());
		settings.setValue("tag", ui->lineCommandsTag->text());
		settings.beginGroup("SQL");
			settings.setValue("driver", ui->comboCommandsSqlDriver->currentText());
			settings.setValue("host", ui->lineCommandsSqlHost->text());
			settings.setValue("user", ui->lineCommandsSqlUser->text());
			settings.setValue("password", ui->lineCommandsSqlPassword->text());
			settings.setValue("database", ui->lineCommandsSqlDatabase->text());
			settings.setValue("before", ui->lineCommandsSqlBefore->text());
			settings.setValue("image", ui->lineCommandsSqlImage->text());
			settings.setValue("tag", ui->lineCommandsSqlTag->text());
			settings.setValue("after", ui->lineCommandsSqlAfter->text());
		settings.endGroup();
	settings.endGroup();

	if (settings.value("Proxy/use", false).toBool())
	{
		QNetworkProxy::ProxyType type = settings.value("Proxy/type", "http") == "http" ? QNetworkProxy::HttpProxy : QNetworkProxy::Socks5Proxy;
		QNetworkProxy proxy(type, settings.value("Proxy/hostName").toString(), settings.value("Proxy/port").toInt());
		QNetworkProxy::setApplicationProxy(proxy);
		log(tr("Activation du proxy général sur l'hôte \"%1\" et le port %2.").arg(settings.value("Proxy/hostName").toString()).arg(settings.value("Proxy/port").toInt()));
	}
	else if (QNetworkProxy::applicationProxy().type() != QNetworkProxy::NoProxy)
	{
		QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
		log(tr("Désactivation du proxy général."));
	}

	if (settings.value("language", "English").toString() != ui->comboLanguages->currentText())
	{
		settings.setValue("language", ui->comboLanguages->currentText());
		emit languageChanged(ui->comboLanguages->currentText());
	}

	settings.sync();
	emit settingsChanged();
}
