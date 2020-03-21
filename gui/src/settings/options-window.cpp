#include "settings/options-window.h"
#include <QColorDialog>
#include <QFileDialog>
#include <QFontDialog>
#include <QNetworkProxy>
#include <QSignalMapper>
#include <QSqlDatabase>
#include <ui_options-window.h>
#include <algorithm>
#include "functions.h"
#include "helpers.h"
#include "language-loader.h"
#include "logger.h"
#include "models/profile.h"
#include "models/site.h"
#include "reverse-search/reverse-search-loader.h"
#include "settings/condition-window.h"
#include "settings/custom-window.h"
#include "settings/filename-window.h"
#include "settings/log-window.h"
#include "settings/token-settings-widget.h"
#include "settings/web-service-window.h"
#include "theme-loader.h"


OptionsWindow::OptionsWindow(Profile *profile, QWidget *parent)
	: QDialog(parent), ui(new Ui::OptionsWindow), m_profile(profile)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);

	QSettings *settings = profile->getSettings();

	ui->splitter->setSizes(QList<int>() << 160 << ui->stackedWidget->sizeHint().width());
	ui->splitter->setStretchFactor(0, 0);
	ui->splitter->setStretchFactor(1, 1);

	LanguageLoader languageLoader(savePath("languages/", true, false));
	QMap<QString, QString> languages = languageLoader.getAllLanguages();
	for (auto it = languages.constBegin(); it != languages.constEnd(); ++it) {
		ui->comboLanguages->addItem(it.value(), it.key());
	}

	ui->comboLanguages->setCurrentText(languages[settings->value("language", "English").toString()]);
	ui->lineWhitelist->setText(settings->value("whitelistedtags").toString());
	ui->lineAdd->setText(settings->value("add").toString());
	ui->lineGlobalPostFilter->setText(settings->value("globalPostFilter").toString());
	ui->checkGlobalPostFilterExplicit->setChecked(settings->value("globalPostFilterExplicit", false).toBool());
	QStringList wl = QStringList() << "never" << "image" << "page";
	ui->comboWhitelist->setCurrentIndex(wl.indexOf(settings->value("whitelist_download", "image").toString()));
	ui->lineIgnored->setText(settings->value("ignoredtags").toString());
	QStringList starts = QStringList() << "none" << "loadfirst" << "restore";
	ui->comboStart->setCurrentIndex(starts.indexOf(settings->value("start", "none").toString()));
	ui->spinHideFavorites->setValue(settings->value("hidefavorites", 20).toInt());
	ui->checkAutodownload->setChecked(settings->value("autodownload", false).toBool());
	ui->checkHideBlacklisted->setChecked(settings->value("hideblacklisted", false).toBool());
	ui->checkShowTagWarning->setChecked(settings->value("showtagwarning", true).toBool());
	ui->checkShowWarnings->setChecked(settings->value("showwarnings", true).toBool());
	ui->checkGetUnloadedPages->setChecked(settings->value("getunloadedpages", false).toBool());
	ui->checkInvertToggle->setChecked(settings->value("invertToggle", false).toBool());
	ui->checkConfirmClose->setChecked(settings->value("confirm_close", true).toBool());
	ui->checkSendUsageData->setChecked(settings->value("send_usage_data", true).toBool());
	QList<int> checkForUpdates = QList<int>() << 0 << 24 * 60 * 60 << 7 * 24 * 60 * 60 << 30 * 24 * 60 * 60 << -1;
	ui->comboCheckForUpdates->setCurrentIndex(checkForUpdates.indexOf(settings->value("check_for_updates", 24 * 60 * 60).toInt()));
	ui->lineTempPathOverride->setText(settings->value("tempPathOverride").toString());

	ui->spinImagesPerPage->setValue(settings->value("limit", 20).toInt());
	ui->spinColumns->setValue(settings->value("columns", 1).toInt());
	QStringList sources = QStringList() << "xml" << "json" << "regex" << "rss";
	ui->comboSource1->setCurrentIndex(sources.indexOf(settings->value("source_1", "xml").toString()));
	ui->comboSource2->setCurrentIndex(sources.indexOf(settings->value("source_2", "json").toString()));
	ui->comboSource3->setCurrentIndex(sources.indexOf(settings->value("source_3", "regex").toString()));
	ui->comboSource4->setCurrentIndex(sources.indexOf(settings->value("source_4", "rss").toString()));
	ui->spinAutoTagAdd->setValue(settings->value("tagsautoadd", 10).toInt());

	QList<ConditionalFilename> filenames = getFilenames(settings);
	m_filenamesConditions = QList<QLineEdit*>();
	m_filenamesFilenames = QList<QLineEdit*>();
	for (const auto &fn : filenames) {
		auto leCondition = new QLineEdit(fn.condition);
		auto leFilename = new QLineEdit(fn.filename.format());
		auto leFolder = new QLineEdit(fn.path);

		m_filenamesConditions.append(leCondition);
		m_filenamesFilenames.append(leFilename);
		m_filenamesFolders.append(leFolder);

		auto *layout = new QHBoxLayout;
		layout->addWidget(leCondition);
		layout->addWidget(leFilename);
		layout->addWidget(leFolder);
		ui->layoutConditionals->addLayout(layout);
	}
	QStringList types = QStringList() << "text" << "icon" << "both" << "hide";
	ui->comboSources->setCurrentIndex(types.indexOf(settings->value("Sources/Types", "icon").toString()));
	int letterCount = settings->value("Sources/Letters", 3).toInt();
	ui->comboSourcesLetters->setCurrentIndex((letterCount < 0 ? 1 : 0) + (letterCount < -1 ? 1 : 0));
	ui->spinSourcesLetters->setValue(letterCount < 0 ? 3 : letterCount);
	ui->checkPreloadAllTabs->setChecked(settings->value("preloadAllTabs", false).toBool());

	QStringList ftypes = QStringList() << "ind" << "in" << "id" << "nd" << "i" << "n" << "d";
	ui->comboFavoritesDisplay->setCurrentIndex(ftypes.indexOf(settings->value("favorites_display", "ind").toString()));

	// Log
	settings->beginGroup("Log");
		ui->checkShowLog->setChecked(settings->value("show", true).toBool());
	settings->endGroup();

	// Blacklist
	ui->textBlacklist->setPlainText(profile->getBlacklist().toString());
	ui->checkDownloadBlacklisted->setChecked(settings->value("downloadblacklist", false).toBool());

	// Ignored tags
	ui->textIgnoredTags->setPlainText(profile->getIgnored().join('\n'));

	// Monitoring
	settings->beginGroup("Monitoring");
		ui->spinMonitoringStartupDelay->setValue(settings->value("startupDelay", 0).toInt());
		ui->checkMonitoringEnableTray->setChecked(settings->value("enableTray", false).toBool());
		ui->checkMonitoringMinimizeToTray->setChecked(settings->value("minimizeToTray", false).toBool());
		ui->checkMonitoringCloseToTray->setChecked(settings->value("closeToTray", false).toBool());
	settings->endGroup();

	ui->checkResizeInsteadOfCropping->setChecked(settings->value("resizeInsteadOfCropping", true).toBool());
	ui->spinThumbnailUpscale->setValue(qRound(settings->value("thumbnailUpscale", 1.0).toDouble() * 100));
	ui->checkAutocompletion->setChecked(settings->value("autocompletion", true).toBool());
	ui->checkUseregexfortags->setChecked(settings->value("useregexfortags", true).toBool());
	QStringList infiniteScroll = QStringList() << "disabled" << "button" << "scroll";
	ui->comboInfiniteScroll->setCurrentIndex(infiniteScroll.indexOf(settings->value("infiniteScroll", "disabled").toString()));
	ui->checkInfiniteScrollRememberPage->setChecked(settings->value("infiniteScrollRememberPage", false).toBool());

	// Resize
	settings->beginGroup("ImageSize");
		ui->spinResizeMaxWidth->setValue(settings->value("maxWidth", 1000).toInt());
		ui->checkResizeMaxWidth->setChecked(settings->value("maxWidthEnabled", false).toBool());
		ui->spinResizeMaxHeight->setValue(settings->value("maxHeight", 1000).toInt());
		ui->checkResizeMaxHeight->setChecked(settings->value("maxHeightEnabled", false).toBool());
	settings->endGroup();

	// External log files
	showLogFiles(settings);

	// Web services
	ReverseSearchLoader loader(settings);
	m_webServices = loader.getAllReverseSearchEngines();
	showWebServices();

	ui->comboBatchEnd->setCurrentIndex(settings->value("Batch/end", 0).toInt());
	settings->beginGroup("Save");
		ui->spinAutomaticRetries->setValue(settings->value("automaticretries", 0).toInt());
		ui->checkDownloadOriginals->setChecked(settings->value("downloadoriginals", true).toBool());
		ui->checkSampleFallback->setChecked(settings->value("samplefallback", true).toBool());
		ui->checkReplaceBlanks->setChecked(settings->value("replaceblanks", false).toBool());
		ui->checkKeepDate->setChecked(settings->value("keepDate", true).toBool());
		ui->checkSaveHeaderDetection->setChecked(settings->value("headerDetection", true).toBool());
		ui->lineFolder->setText(settings->value("path_real").toString());
		ui->lineFolderFavorites->setText(settings->value("path_favorites").toString());
		QStringList md5Duplicates = QStringList() << "save" << "copy" << "move" << "link" << "ignore";
		ui->comboMd5Duplicates->setCurrentIndex(md5Duplicates.indexOf(settings->value("md5Duplicates", "save").toString()));
		ui->checkKeepDeletedMd5->setChecked(settings->value("keepDeletedMd5", false).toBool());
		QStringList multipleFiles = QStringList() << "copy" << "link";
		ui->comboMultipleFiles->setCurrentIndex(multipleFiles.indexOf(settings->value("multiple_files", "copy").toString()));

		ui->lineFilename->setText(settings->value("filename_real").toString());
		ui->lineFavorites->setText(settings->value("filename_favorites").toString());
		ui->lineSeparator->setText(settings->value("separator", " ").toString());
		ui->checkNoJpeg->setChecked(settings->value("noJpeg", true).toBool());


		// Build the "tags" settings
		auto tagsTree = ui->treeWidget->invisibleRootItem()->child(2)->child(4);
		tagsTree->addChild(new QTreeWidgetItem(QStringList() << "Artist", tagsTree->type()));
		tagsTree->addChild(new QTreeWidgetItem(QStringList() << "Copyright", tagsTree->type()));
		tagsTree->addChild(new QTreeWidgetItem(QStringList() << "Character", tagsTree->type()));
		tagsTree->addChild(new QTreeWidgetItem(QStringList() << "Model", tagsTree->type()));
		tagsTree->addChild(new QTreeWidgetItem(QStringList() << "Photo set", tagsTree->type()));
		tagsTree->addChild(new QTreeWidgetItem(QStringList() << "Species", tagsTree->type()));
		tagsTree->addChild(new QTreeWidgetItem(QStringList() << "Meta", tagsTree->type()));
		m_tokenSettings.append(new TokenSettingsWidget(settings, "artist", false, "anonymous", "multiple artists", this));
		m_tokenSettings.append(new TokenSettingsWidget(settings, "copyright", true, "misc", "crossover", this));
		m_tokenSettings.append(new TokenSettingsWidget(settings, "character", false, "unknown", "group", this));
		m_tokenSettings.append(new TokenSettingsWidget(settings, "model", false, "unknown", "multiple", this));
		m_tokenSettings.append(new TokenSettingsWidget(settings, "photo_set", false, "unknown", "multiple", this));
		m_tokenSettings.append(new TokenSettingsWidget(settings, "species", false, "unknown", "multiple", this));
		m_tokenSettings.append(new TokenSettingsWidget(settings, "meta", false, "none", "multiple", this));
		for (int i = 0; i < m_tokenSettings.count(); ++i) {
			ui->stackedWidget->insertWidget(i + 8, m_tokenSettings[i]);
		}

		ui->spinLimit->setValue(settings->value("limit", 0).toInt());
		ui->spinSimultaneous->setValue(settings->value("simultaneous", 1).toInt());
	settings->endGroup();

	// Custom tokens
	QMap<QString, QStringList> customs = getCustoms(settings);
	m_customNames = QList<QLineEdit*>();
	m_customTags = QList<QLineEdit*>();
	int i = 0;
	for (auto it = customs.constBegin(); it != customs.constEnd(); ++it) {
		auto *leName = new QLineEdit(it.key());
		auto *leTags = new QLineEdit(it.value().join(" "));
		m_customNames.append(leName);
		m_customTags.append(leTags);
		ui->layoutCustom->insertRow(i++, leName, leTags);
	}

	// Themes
	ThemeLoader themeLoader(savePath("themes/", true));
	QStringList themes = themeLoader.getAllThemes();
	for (const QString &theme : themes) {
		ui->comboTheme->addItem(theme, theme);
	}
	ui->comboTheme->setCurrentText(settings->value("theme", "Default").toString());

	ui->checkSingleDetailsWindow->setChecked(settings->value("Zoom/singleWindow", false).toBool());
	QStringList positions = QStringList() << "top" << "left" << "auto";
	ui->comboTagsPosition->setCurrentIndex(positions.indexOf(settings->value("tagsposition", "top").toString()));
	ui->spinPreload->setValue(settings->value("preload", 0).toInt());
	ui->spinSlideshow->setValue(settings->value("slideshow", 0).toInt());
	ui->checkResultsScrollArea->setChecked(settings->value("resultsScrollArea", true).toBool());
	ui->checkResultsFixedWidthLayout->setChecked(settings->value("resultsFixedWidthLayout", false).toBool());
	ui->checkImageCloseMiddleClick->setChecked(settings->value("imageCloseMiddleClick", true).toBool());
	ui->checkImageNavigateScroll->setChecked(settings->value("imageNavigateScroll", true).toBool());
	ui->checkZoomShowTagCount->setChecked(settings->value("Zoom/showTagCount", false).toBool());
	ui->checkZoomViewSamples->setChecked(settings->value("Zoom/viewSamples", false).toBool());
	QStringList imageTagOrder = QStringList() << "type" << "name" << "count";
	ui->comboImageTagOrder->setCurrentIndex(imageTagOrder.indexOf(settings->value("Zoom/tagOrder", "type").toString()));
	QStringList positionsV = QStringList() << "top" << "center" << "bottom";
	QStringList positionsH = QStringList() << "left" << "center" << "right";
	ui->comboImagePositionImageV->setCurrentIndex(positionsV.indexOf(settings->value("imagePositionImageV", "center").toString()));
	ui->comboImagePositionImageH->setCurrentIndex(positionsH.indexOf(settings->value("imagePositionImageH", "left").toString()));
	ui->comboImagePositionAnimationV->setCurrentIndex(positionsV.indexOf(settings->value("imagePositionAnimationV", "center").toString()));
	ui->comboImagePositionAnimationH->setCurrentIndex(positionsH.indexOf(settings->value("imagePositionAnimationH", "left").toString()));
	ui->comboImagePositionVideoV->setCurrentIndex(positionsV.indexOf(settings->value("imagePositionVideoV", "center").toString()));
	ui->comboImagePositionVideoH->setCurrentIndex(positionsH.indexOf(settings->value("imagePositionVideoH", "left").toString()));
	ui->lineImageBackgroundColor->setText(settings->value("imageBackgroundColor", QString()).toString());

	settings->beginGroup("Coloring");
		settings->beginGroup("Colors");
			ui->lineColoringArtists->setText(settings->value("artists", "#aa0000").toString());
			ui->lineColoringCircles->setText(settings->value("circles", "#55bbff").toString());
			ui->lineColoringCopyrights->setText(settings->value("copyrights", "#aa00aa").toString());
			ui->lineColoringCharacters->setText(settings->value("characters", "#00aa00").toString());
			ui->lineColoringSpecies->setText(settings->value("species", "#ee6600").toString());
			ui->lineColoringMetas->setText(settings->value("metas", "#ee6600").toString());
			ui->lineColoringModels->setText(settings->value("models", "#0000ee").toString());
			ui->lineColoringGenerals->setText(settings->value("generals", "#000000").toString());
			ui->lineColoringFavorites->setText(settings->value("favorites", "#ffc0cb").toString());
			ui->lineColoringKeptForLater->setText(settings->value("keptForLater", "#000000").toString());
			ui->lineColoringBlacklisteds->setText(settings->value("blacklisteds", "#000000").toString());
			ui->lineColoringIgnoreds->setText(settings->value("ignoreds", "#999999").toString());
		settings->endGroup();
		settings->beginGroup("Fonts");
			ui->lineColoringArtists->setFont(qFontFromString(settings->value("artists").toString()));
			ui->lineColoringCircles->setFont(qFontFromString(settings->value("circles").toString()));
			ui->lineColoringCopyrights->setFont(qFontFromString(settings->value("copyrights").toString()));
			ui->lineColoringCharacters->setFont(qFontFromString(settings->value("characters").toString()));
			ui->lineColoringSpecies->setFont(qFontFromString(settings->value("species").toString()));
			ui->lineColoringMetas->setFont(qFontFromString(settings->value("metas").toString()));
			ui->lineColoringModels->setFont(qFontFromString(settings->value("models").toString()));
			ui->lineColoringGenerals->setFont(qFontFromString(settings->value("generals").toString()));
			ui->lineColoringFavorites->setFont(qFontFromString(settings->value("favorites").toString()));
			ui->lineColoringKeptForLater->setFont(qFontFromString(settings->value("keptForLater").toString()));
			ui->lineColoringBlacklisteds->setFont(qFontFromString(settings->value("blacklisteds").toString()));
			ui->lineColoringIgnoreds->setFont(qFontFromString(settings->value("ignoreds").toString()));
		settings->endGroup();
	settings->endGroup();

	settings->beginGroup("Margins");
		ui->spinMainMargins->setValue(settings->value("main", 10).toInt());
		ui->spinHorizontalMargins->setValue(settings->value("horizontal", 6).toInt());
		ui->spinVerticalMargins->setValue(settings->value("vertical", 6).toInt());
	settings->endGroup();
	ui->spinServerBorders->setValue(settings->value("serverBorder", 0).toInt());
	ui->lineBorderColor->setText(settings->value("serverBorderColor", "#000000").toString());
	ui->spinBorders->setValue(settings->value("borders", 3).toInt());

	settings->beginGroup("Proxy");
		ui->checkProxyUse->setChecked(settings->value("use", false).toBool());
		ui->checkProxyUseSystem->setChecked(settings->value("useSystem", false).toBool());
		QStringList ptypes = QStringList() << "http" << "socks5";
		ui->comboProxyType->setCurrentIndex(ptypes.indexOf(settings->value("type", "http").toString()));
		ui->widgetProxy->setEnabled(settings->value("use", false).toBool());
		ui->lineProxyHostName->setText(settings->value("hostName").toString());
		ui->spinProxyPort->setValue(settings->value("port").toInt());
		ui->lineProxyUser->setText(settings->value("user").toString());
		ui->lineProxyPassword->setText(settings->value("password").toString());
	settings->endGroup();

	settings->beginGroup("Exec");
		ui->lineCommandsTagBefore->setText(settings->value("tag_before").toString());
		ui->lineCommandsImage->setText(settings->value("image").toString());
		ui->lineCommandsTagAfter->setText(settings->value("tag_after", settings->value("tag").toString()).toString());
		settings->beginGroup("SQL");
			ui->comboCommandsSqlDriver->addItems(QSqlDatabase::drivers());
			ui->comboCommandsSqlDriver->setCurrentIndex(QSqlDatabase::drivers().indexOf(settings->value("driver", "QMYSQL").toString()));
			ui->lineCommandsSqlHost->setText(settings->value("host").toString());
			ui->lineCommandsSqlUser->setText(settings->value("user").toString());
			ui->lineCommandsSqlPassword->setText(settings->value("password").toString());
			ui->lineCommandsSqlDatabase->setText(settings->value("database").toString());
			ui->lineCommandsSqlBefore->setText(settings->value("before").toString());
			ui->lineCommandsSqlTagBefore->setText(settings->value("tag_before").toString());
			ui->lineCommandsSqlImage->setText(settings->value("image").toString());
			ui->lineCommandsSqlTagAfter->setText(settings->value("tag_after", settings->value("tag").toString()).toString());
			ui->lineCommandsSqlAfter->setText(settings->value("after").toString());
		settings->endGroup();
	settings->endGroup();

	connect(this, &QDialog::accepted, this, &OptionsWindow::save);
}

OptionsWindow::~OptionsWindow()
{
	delete ui;
}

void OptionsWindow::on_comboSourcesLetters_currentIndexChanged(int i)
{ ui->spinSourcesLetters->setDisabled(i > 0); }

void OptionsWindow::on_buttonFolder_clicked()
{
	QString folder = QFileDialog::getExistingDirectory(this, tr("Choose a save folder"), ui->lineFolder->text());
	if (!folder.isEmpty()) {
		ui->lineFolder->setText(folder);
	}
}
void OptionsWindow::on_buttonFolderFavorites_clicked()
{
	QString folder = QFileDialog::getExistingDirectory(this, tr("Choose a save folder for favorites"), ui->lineFolderFavorites->text());
	if (!folder.isEmpty()) {
		ui->lineFolderFavorites->setText(folder);
	}
}
void OptionsWindow::on_buttonTempPathOverride_clicked()
{
	QString folder = QFileDialog::getExistingDirectory(this, tr("Choose a temporary folder"), ui->lineTempPathOverride->text());
	if (!folder.isEmpty()) {
		ui->lineTempPathOverride->setText(folder);
	}
}

void OptionsWindow::on_buttonFilenamePlus_clicked()
{
	FilenameWindow *fw = new FilenameWindow(m_profile, ui->lineFilename->text(), this);
	connect(fw, &FilenameWindow::validated, ui->lineFilename, &QLineEdit::setText);
	fw->show();
}
void OptionsWindow::on_buttonFavoritesPlus_clicked()
{
	FilenameWindow *fw = new FilenameWindow(m_profile, ui->lineFavorites->text(), this);
	connect(fw, &FilenameWindow::validated, ui->lineFavorites, &QLineEdit::setText);
	fw->show();
}

void OptionsWindow::on_buttonCustom_clicked()
{
	auto *cw = new CustomWindow(this);
	connect(cw, &CustomWindow::validated, this, &OptionsWindow::addCustom);
	cw->show();
}
void OptionsWindow::addCustom(const QString &name, const QString &tags)
{
	auto *leName = new QLineEdit(name);
	auto *leTags = new QLineEdit(tags);
	ui->layoutCustom->insertRow(m_customNames.size(), leName, leTags);
	m_customNames.append(leName);
	m_customTags.append(leTags);
}
void OptionsWindow::on_buttonFilenames_clicked()
{
	auto *cw = new ConditionWindow();
	connect(cw, &ConditionWindow::validated, this, &OptionsWindow::addFilename);
	cw->show();
}
void OptionsWindow::addFilename(const QString &condition, const QString &filename, const QString &folder)
{
	auto *leCondition = new QLineEdit(condition);
	auto *leFilename = new QLineEdit(filename);
	auto *leFolder = new QLineEdit(folder);

	m_filenamesConditions.append(leCondition);
	m_filenamesFilenames.append(leFilename);
	m_filenamesFolders.append(leFolder);

	auto *layout = new QHBoxLayout(this);
	layout->addWidget(leCondition);
	layout->addWidget(leFilename);
	layout->addWidget(leFolder);
	ui->layoutConditionals->addLayout(layout);
}


void OptionsWindow::showLogFiles(QSettings *settings)
{
	clearLayout(ui->layoutLogFiles);

	auto logFiles = getExternalLogFiles(settings);
	auto *mapperEditLogFile = new QSignalMapper(this);
	auto *mapperRemoveLogFile = new QSignalMapper(this);
	connect(mapperEditLogFile, SIGNAL(mapped(int)), this, SLOT(editLogFile(int)));
	connect(mapperRemoveLogFile, SIGNAL(mapped(int)), this, SLOT(removeLogFile(int)));
	for (auto it = logFiles.constBegin(); it != logFiles.constEnd(); ++it) {
		const int i = it.key();
		auto logFile = it.value();

		auto *label = new QLabel(logFile["name"].toString());
		label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		ui->layoutLogFiles->addWidget(label, i, 0);

		auto *buttonEdit = new QPushButton(tr("Edit"));
		mapperEditLogFile->setMapping(buttonEdit, i);
		connect(buttonEdit, SIGNAL(clicked(bool)), mapperEditLogFile, SLOT(map()));
		ui->layoutLogFiles->addWidget(buttonEdit, i, 1);

		auto *buttonDelete = new QPushButton(tr("Remove"));
		mapperRemoveLogFile->setMapping(buttonDelete, i);
		connect(buttonDelete, SIGNAL(clicked(bool)), mapperRemoveLogFile, SLOT(map()));
		ui->layoutLogFiles->addWidget(buttonDelete, i, 2);
	}
}

void OptionsWindow::addLogFile()
{
	auto *logWindow = new LogWindow(-1, m_profile, this);
	connect(logWindow, &LogWindow::validated, this, &OptionsWindow::setLogFile);
	logWindow->show();
}

void OptionsWindow::editLogFile(int index)
{
	auto *logWindow = new LogWindow(index, m_profile, this);
	connect(logWindow, &LogWindow::validated, this, &OptionsWindow::setLogFile);
	logWindow->show();
}

void OptionsWindow::removeLogFile(int index)
{
	QSettings *settings = m_profile->getSettings();
	settings->beginGroup("LogFiles");
	settings->beginGroup(QString::number(index));
	for (const QString &key : settings->childKeys()) {
		settings->remove(key);
	}
	settings->endGroup();
	settings->endGroup();

	showLogFiles(settings);
}

void OptionsWindow::setLogFile(int index, const QMap<QString, QVariant> &logFile)
{
	QSettings *settings = m_profile->getSettings();
	settings->beginGroup("LogFiles");

	if (index < 0) {
		auto childGroups = settings->childGroups();
		if (childGroups.isEmpty()) {
			index = 0;
		} else {
			index = childGroups.last().toInt() + 1;
		}
	}

	settings->beginGroup(QString::number(index));

	for (auto it = logFile.constBegin(); it != logFile.constEnd(); ++it) {
		settings->setValue(it.key(), it.value());
	}

	settings->endGroup();
	settings->endGroup();

	showLogFiles(settings);
}


void OptionsWindow::showWebServices()
{
	clearLayout(ui->layoutWebServices);

	auto *mapperEditWebService = new QSignalMapper(this);
	auto *mapperRemoveWebService = new QSignalMapper(this);
	auto *mapperMoveUpWebService = new QSignalMapper(this);
	auto *mapperMoveDownWebService = new QSignalMapper(this);
	connect(mapperEditWebService, SIGNAL(mapped(int)), this, SLOT(editWebService(int)));
	connect(mapperRemoveWebService, SIGNAL(mapped(int)), this, SLOT(removeWebService(int)));
	connect(mapperMoveUpWebService, SIGNAL(mapped(int)), this, SLOT(moveUpWebService(int)));
	connect(mapperMoveDownWebService, SIGNAL(mapped(int)), this, SLOT(moveDownWebService(int)));

	m_webServicesIds.clear();
	for (int j = 0; j < m_webServices.count(); ++j) {
		auto webService = m_webServices[j];
		int id = webService.id();
		m_webServicesIds.insert(id, j);

		QIcon icon = webService.icon();
		QLabel *labelIcon = new QLabel();
		labelIcon->setPixmap(icon.pixmap(QSize(16, 16)));
		labelIcon->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
		ui->layoutWebServices->addWidget(labelIcon, j, 0);

		QLabel *label = new QLabel(webService.name());
		label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		ui->layoutWebServices->addWidget(label, j, 1);

		if (j > 0) {
			QPushButton *buttonMoveUp = new QPushButton(QIcon(":/images/icons/arrow-up.png"), QString());
			mapperMoveUpWebService->setMapping(buttonMoveUp, id);
			connect(buttonMoveUp, SIGNAL(clicked(bool)), mapperMoveUpWebService, SLOT(map()));
			ui->layoutWebServices->addWidget(buttonMoveUp, j, 2);
		}

		if (j < m_webServices.count() - 1) {
			QPushButton *buttonMoveDown = new QPushButton(QIcon(":/images/icons/arrow-down.png"), QString());
			mapperMoveDownWebService->setMapping(buttonMoveDown, id);
			connect(buttonMoveDown, SIGNAL(clicked(bool)), mapperMoveDownWebService, SLOT(map()));
			ui->layoutWebServices->addWidget(buttonMoveDown, j, 3);
		}

		QPushButton *buttonEdit = new QPushButton(tr("Edit"));
		mapperEditWebService->setMapping(buttonEdit, id);
		connect(buttonEdit, SIGNAL(clicked(bool)), mapperEditWebService, SLOT(map()));
		ui->layoutWebServices->addWidget(buttonEdit, j, 4);

		QPushButton *buttonDelete = new QPushButton(tr("Remove"));
		mapperRemoveWebService->setMapping(buttonDelete, id);
		connect(buttonDelete, SIGNAL(clicked(bool)), mapperRemoveWebService, SLOT(map()));
		ui->layoutWebServices->addWidget(buttonDelete, j, 5);
	}
}

void OptionsWindow::addWebService()
{
	auto *wsWindow = new WebServiceWindow(nullptr, this);
	connect(wsWindow, &WebServiceWindow::validated, this, &OptionsWindow::setWebService);
	wsWindow->show();
}

void OptionsWindow::editWebService(int id)
{
	int pos = m_webServicesIds[id];
	auto *wsWindow = new WebServiceWindow(&m_webServices[pos], this);
	connect(wsWindow, &WebServiceWindow::validated, this, &OptionsWindow::setWebService);
	wsWindow->show();
}

void OptionsWindow::removeWebService(int id)
{
	m_webServices.removeAt(m_webServicesIds[id]);

	// Delete favicon file
	QFile(savePath("webservices/") + QString::number(id) + ".ico").remove();

	// Remove WebService config
	QSettings *settings = m_profile->getSettings();
	settings->beginGroup("WebServices");
	settings->beginGroup(QString::number(id));
	settings->remove("");
	settings->endGroup();
	settings->endGroup();

	showWebServices();
}

void OptionsWindow::setWebService(ReverseSearchEngine rse, const QByteArray &favicon)
{
	const bool isNew = rse.id() < 0;

	// Generate new ID for new web services
	if (isNew) {
		int maxOrder = 0;
		int maxId = 0;
		for (const ReverseSearchEngine &ws : qAsConst(m_webServices)) {
			if (ws.id() > maxId) {
				maxId = ws.id();
			}
			if (ws.order() > maxOrder) {
				maxOrder = ws.order();
			}
		}

		rse.setId(maxId + 1);
		rse.setOrder(maxOrder + 1);
	}

	// Write icon information to disk
	if (!favicon.isEmpty()) {
		QString faviconPath = savePath("webservices/") + QString::number(rse.id()) + ".ico";
		QFile f(faviconPath);
		if (f.open(QFile::WriteOnly)) {
			f.write(favicon);
			f.close();
		}
		rse = ReverseSearchEngine(rse.id(), faviconPath, rse.name(), rse.tpl(), rse.order());
	}

	if (isNew) {
		m_webServices.append(rse);
	} else {
		m_webServices[m_webServicesIds[rse.id()]] = rse;
	}

	showWebServices();
}

void OptionsWindow::moveUpWebService(int id)
{
	const int i = m_webServicesIds[id];
	if (i == 0) {
		return;
	}

	swapWebServices(i, i - 1);
}

void OptionsWindow::moveDownWebService(int id)
{
	const int i = m_webServicesIds[id];
	if (i == m_webServicesIds.count() - 1) {
		return;
	}

	swapWebServices(i, i + 1);
}

bool sortByOrder(const ReverseSearchEngine &a, const ReverseSearchEngine &b)
{ return a.order() < b.order(); }
void OptionsWindow::swapWebServices(int a, int b)
{
	const int pos = m_webServices[b].order();
	m_webServices[b].setOrder(m_webServices[a].order());
	m_webServices[a].setOrder(pos);

	// Re-order web services
	std::sort(m_webServices.begin(), m_webServices.end(), sortByOrder);
	m_webServicesIds.clear();
	for (int i = 0; i < m_webServices.count(); ++i) {
		m_webServicesIds.insert(m_webServices[i].id(), i);
	}

	showWebServices();
}


void OptionsWindow::setColor(QLineEdit *lineEdit, bool button)
{
	const QString text = lineEdit->text();
	QColor color = button
		? QColorDialog::getColor(QColor(text), this, tr("Choose a color"))
		: QColor(text);

	if (color.isValid()) {
		lineEdit->setText(button ? color.name() : text);
		lineEdit->setStyleSheet("color:" + color.name());
	} else if (!button) {
		lineEdit->setStyleSheet("color:#000000");
	}
}

void OptionsWindow::setFont(QLineEdit *lineEdit)
{
	bool ok = false;
	const QFont police = QFontDialog::getFont(&ok, lineEdit->font(), this, tr("Choose a font"));

	if (ok) {
		lineEdit->setFont(police);
	}
}

void OptionsWindow::on_lineColoringArtists_textChanged()
{ setColor(ui->lineColoringArtists); }
void OptionsWindow::on_lineColoringCircles_textChanged()
{ setColor(ui->lineColoringCircles); }
void OptionsWindow::on_lineColoringCopyrights_textChanged()
{ setColor(ui->lineColoringCopyrights); }
void OptionsWindow::on_lineColoringCharacters_textChanged()
{ setColor(ui->lineColoringCharacters); }
void OptionsWindow::on_lineColoringSpecies_textChanged()
{ setColor(ui->lineColoringSpecies); }
void OptionsWindow::on_lineColoringMetas_textChanged()
{ setColor(ui->lineColoringMetas); }
void OptionsWindow::on_lineColoringModels_textChanged()
{ setColor(ui->lineColoringModels); }
void OptionsWindow::on_lineColoringGenerals_textChanged()
{ setColor(ui->lineColoringGenerals); }
void OptionsWindow::on_lineColoringFavorites_textChanged()
{ setColor(ui->lineColoringFavorites); }
void OptionsWindow::on_lineColoringKeptForLater_textChanged()
{ setColor(ui->lineColoringKeptForLater); }
void OptionsWindow::on_lineColoringBlacklisteds_textChanged()
{ setColor(ui->lineColoringBlacklisteds); }
void OptionsWindow::on_lineColoringIgnoreds_textChanged()
{ setColor(ui->lineColoringIgnoreds); }
void OptionsWindow::on_lineBorderColor_textChanged()
{ setColor(ui->lineBorderColor); }

void OptionsWindow::on_buttonColoringArtistsColor_clicked()
{ setColor(ui->lineColoringArtists, true); }
void OptionsWindow::on_buttonColoringCirclesColor_clicked()
{ setColor(ui->lineColoringCircles, true); }
void OptionsWindow::on_buttonColoringCopyrightsColor_clicked()
{ setColor(ui->lineColoringCopyrights, true); }
void OptionsWindow::on_buttonColoringCharactersColor_clicked()
{ setColor(ui->lineColoringCharacters, true); }
void OptionsWindow::on_buttonColoringSpeciesColor_clicked()
{ setColor(ui->lineColoringSpecies, true); }
void OptionsWindow::on_buttonColoringMetasColor_clicked()
{ setColor(ui->lineColoringMetas, true); }
void OptionsWindow::on_buttonColoringModelsColor_clicked()
{ setColor(ui->lineColoringModels, true); }
void OptionsWindow::on_buttonColoringGeneralsColor_clicked()
{ setColor(ui->lineColoringGenerals, true); }
void OptionsWindow::on_buttonColoringFavoritesColor_clicked()
{ setColor(ui->lineColoringFavorites, true); }
void OptionsWindow::on_buttonColoringKeptForLaterColor_clicked()
{ setColor(ui->lineColoringKeptForLater, true); }
void OptionsWindow::on_buttonColoringBlacklistedsColor_clicked()
{ setColor(ui->lineColoringBlacklisteds, true); }
void OptionsWindow::on_buttonColoringIgnoredsColor_clicked()
{ setColor(ui->lineColoringIgnoreds, true); }
void OptionsWindow::on_buttonBorderColor_clicked()
{ setColor(ui->lineBorderColor, true); }

void OptionsWindow::on_buttonColoringArtistsFont_clicked()
{ setFont(ui->lineColoringArtists); }
void OptionsWindow::on_buttonColoringCirclesFont_clicked()
{ setFont(ui->lineColoringCircles); }
void OptionsWindow::on_buttonColoringCopyrightsFont_clicked()
{ setFont(ui->lineColoringCopyrights); }
void OptionsWindow::on_buttonColoringCharactersFont_clicked()
{ setFont(ui->lineColoringCharacters); }
void OptionsWindow::on_buttonColoringSpeciesFont_clicked()
{ setFont(ui->lineColoringSpecies); }
void OptionsWindow::on_buttonColoringMetasFont_clicked()
{ setFont(ui->lineColoringMetas); }
void OptionsWindow::on_buttonColoringModelsFont_clicked()
{ setFont(ui->lineColoringModels); }
void OptionsWindow::on_buttonColoringGeneralsFont_clicked()
{ setFont(ui->lineColoringGenerals); }
void OptionsWindow::on_buttonColoringFavoritesFont_clicked()
{ setFont(ui->lineColoringFavorites); }
void OptionsWindow::on_buttonColoringKeptForLaterFont_clicked()
{ setFont(ui->lineColoringKeptForLater); }
void OptionsWindow::on_buttonColoringBlacklistedsFont_clicked()
{ setFont(ui->lineColoringBlacklisteds); }
void OptionsWindow::on_buttonColoringIgnoredsFont_clicked()
{ setFont(ui->lineColoringIgnoreds); }

void OptionsWindow::on_lineImageBackgroundColor_textChanged()
{ setColor(ui->lineImageBackgroundColor); }
void OptionsWindow::on_buttonImageBackgroundColor_clicked()
{ setColor(ui->lineImageBackgroundColor, true); }

void treeWidgetRec(int depth, bool &found, int &index, QTreeWidgetItem *current, QTreeWidgetItem *sel)
{
	if (current == sel) {
		found = true;
		return;
	}
	index++;

	for (int i = 0; i < current->childCount(); ++i) {
		treeWidgetRec(depth + 1, found, index, current->child(i), sel);
		if (found) {
			break;
		}
	}
}

void OptionsWindow::updateContainer(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	Q_UNUSED(previous)

	bool found = false;
	int index = 0;

	for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
		treeWidgetRec(0, found, index, ui->treeWidget->topLevelItem(i), current);
		if (found) {
			break;
		}
	}

	if (found) {
		ui->stackedWidget->setCurrentIndex(index);
	}
}

void OptionsWindow::save()
{
	QSettings *settings = m_profile->getSettings();

	settings->setValue("whitelistedtags", ui->lineWhitelist->text());
	settings->setValue("ignoredtags", ui->lineIgnored->text());
	settings->setValue("add", ui->lineAdd->text());
	settings->setValue("globalPostFilter", ui->lineGlobalPostFilter->text());
	settings->setValue("globalPostFilterExplicit", ui->checkGlobalPostFilterExplicit->isChecked());
	QStringList wl = QStringList() << "never" << "image" << "page";
	settings->setValue("whitelist_download", wl.at(ui->comboWhitelist->currentIndex()));

	settings->setValue("limit", ui->spinImagesPerPage->value());
	settings->setValue("columns", ui->spinColumns->value());
	QStringList sources = QStringList() << "xml" << "json" << "regex" << "rss";
	settings->setValue("source_1", sources.at(ui->comboSource1->currentIndex()));
	settings->setValue("source_2", sources.at(ui->comboSource2->currentIndex()));
	settings->setValue("source_3", sources.at(ui->comboSource3->currentIndex()));
	settings->setValue("source_4", sources.at(ui->comboSource4->currentIndex()));
	settings->setValue("tagsautoadd", ui->spinAutoTagAdd->value());
	QStringList starts = QStringList() << "none" << "loadfirst" << "restore";
	settings->setValue("start", starts.at(ui->comboStart->currentIndex()));
	settings->setValue("hidefavorites", ui->spinHideFavorites->value());
	settings->setValue("autodownload", ui->checkAutodownload->isChecked());
	settings->setValue("hideblacklisted", ui->checkHideBlacklisted->isChecked());
	settings->setValue("showtagwarning", ui->checkShowTagWarning->isChecked());
	settings->setValue("showwarnings", ui->checkShowWarnings->isChecked());
	settings->setValue("getunloadedpages", ui->checkGetUnloadedPages->isChecked());
	settings->setValue("invertToggle", ui->checkInvertToggle->isChecked());
	settings->setValue("confirm_close", ui->checkConfirmClose->isChecked());
	settings->setValue("send_usage_data", ui->checkSendUsageData->isChecked());
	QList<int> checkForUpdates = QList<int>() << 0 << 24 * 60 * 60 << 7 * 24 * 60 * 60 << 30 * 24 * 60 * 60 << -1;
	settings->setValue("check_for_updates", checkForUpdates.at(ui->comboCheckForUpdates->currentIndex()));
	settings->setValue("tempPathOverride", ui->lineTempPathOverride->text());

	settings->beginGroup("Filenames");
		for (int i = 0; i < m_filenamesConditions.size(); i++) {
			if (!m_filenamesConditions.at(i)->text().isEmpty()) {
				settings->setValue(QString::number(i) + "_cond", m_filenamesConditions.at(i)->text());
				settings->setValue(QString::number(i) + "_fn", m_filenamesFilenames.at(i)->text());
				settings->setValue(QString::number(i) + "_dir", m_filenamesFolders.at(i)->text());
			} else {
				settings->remove(QString::number(i) + "_cond");
				settings->remove(QString::number(i) + "_fn");
				settings->remove(QString::number(i) + "_dir");
			}
		}
	settings->endGroup();

	QStringList types = QStringList() << "text" << "icon" << "both" << "hide";
	settings->setValue("Sources/Types", types.at(ui->comboSources->currentIndex()));
	const int i = ui->comboSourcesLetters->currentIndex();
	settings->setValue("Sources/Letters", (i == 0 ? ui->spinSourcesLetters->value() : -i));
	settings->setValue("preloadAllTabs", ui->checkPreloadAllTabs->isChecked());

	QStringList ftypes = QStringList() << "ind" << "in" << "id" << "nd" << "i" << "n" << "d";
	if (settings->value("favorites_display", "ind").toString() != ftypes.at(ui->comboFavoritesDisplay->currentIndex())) {
		settings->setValue("favorites_display", ftypes.at(ui->comboFavoritesDisplay->currentIndex()));
		m_profile->emitFavorite();
	}

	// Log
	settings->beginGroup("Log");
		settings->setValue("show", ui->checkShowLog->isChecked());
	settings->endGroup();

	// Blacklist
	Blacklist blacklist;
	for (const QString &tags : ui->textBlacklist->toPlainText().split("\n", QString::SkipEmptyParts)) {
		blacklist.add(tags.trimmed().split(' ', QString::SkipEmptyParts));
	}
	m_profile->setBlacklistedTags(blacklist);
	settings->setValue("downloadblacklist", ui->checkDownloadBlacklisted->isChecked());

	// Ignored tags
	m_profile->setIgnored(ui->textIgnoredTags->toPlainText().split('\n', QString::SkipEmptyParts));

	// Monitoring
	settings->beginGroup("Monitoring");
		settings->setValue("startupDelay", ui->spinMonitoringStartupDelay->value());
		settings->setValue("enableTray", ui->checkMonitoringEnableTray->isChecked());
		settings->setValue("minimizeToTray", ui->checkMonitoringMinimizeToTray->isChecked());
		settings->setValue("closeToTray", ui->checkMonitoringCloseToTray->isChecked());
	settings->endGroup();

	settings->setValue("resizeInsteadOfCropping", ui->checkResizeInsteadOfCropping->isChecked());
	settings->setValue("thumbnailUpscale", static_cast<double>(ui->spinThumbnailUpscale->value()) / 100.0);
	settings->setValue("autocompletion", ui->checkAutocompletion->isChecked());
	settings->setValue("useregexfortags", ui->checkUseregexfortags->isChecked());
	QStringList infiniteScroll = QStringList() << "disabled" << "button" << "scroll";
	settings->setValue("infiniteScroll", infiniteScroll.at(ui->comboInfiniteScroll->currentIndex()));
	settings->setValue("infiniteScrollRememberPage", ui->checkInfiniteScrollRememberPage->isChecked());

	settings->setValue("Batch/end", ui->comboBatchEnd->currentIndex());
	settings->beginGroup("Save");
		settings->setValue("automaticretries", ui->spinAutomaticRetries->value());
		settings->setValue("downloadoriginals", ui->checkDownloadOriginals->isChecked());
		settings->setValue("samplefallback", ui->checkSampleFallback->isChecked());
		settings->setValue("replaceblanks", ui->checkReplaceBlanks->isChecked());
		settings->setValue("keepDate", ui->checkKeepDate->isChecked());
		settings->setValue("headerDetection", ui->checkSaveHeaderDetection->isChecked());
		settings->setValue("separator", ui->lineSeparator->text());
		settings->setValue("noJpeg", ui->checkNoJpeg->isChecked());
		QString folder = fixFilename("", ui->lineFolder->text());
		settings->setValue("path", folder);
		settings->setValue("path_real", folder);
		QDir pth = QDir(folder);
		if (!pth.exists()) {
			QString op;
			while (!pth.exists() && pth.path() != op) {
				op = pth.path();
				pth.setPath(pth.path().remove(QRegularExpression("/([^/]+)$")));
			}
			if (pth.path() == op) {
				error(this, tr("An error occured creating the save folder."));
			} else {
				pth.mkpath(folder);
			}
		}
		folder = fixFilename("", ui->lineFolderFavorites->text());
		settings->setValue("path_favorites", folder);
		pth = QDir(folder);
		if (!pth.exists()) {
			QString op;
			while (!pth.exists() && pth.path() != op) {
				op = pth.path();
				pth.setPath(pth.path().remove(QRegularExpression("/([^/]+)$")));
			}
			if (pth.path() == op) {
				error(this, tr("An error occured creating the favorites save folder."));
			} else {
				pth.mkpath(folder);
			}
		}
		QStringList md5Duplicates = QStringList() << "save" << "copy" << "move" << "link" << "ignore";
		settings->setValue("md5Duplicates", md5Duplicates.at(ui->comboMd5Duplicates->currentIndex()));
		settings->setValue("keepDeletedMd5", ui->checkKeepDeletedMd5->isChecked());
		QStringList multipleFiles = QStringList() << "copy" << "link";
		settings->setValue("multiple_files", multipleFiles.at(ui->comboMultipleFiles->currentIndex()));

		settings->setValue("filename", ui->lineFilename->text());
		settings->setValue("filename_real", ui->lineFilename->text());
		settings->setValue("filename_favorites", ui->lineFavorites->text());

		for (TokenSettingsWidget *tokenSettings : m_tokenSettings) {
			tokenSettings->save();
		}

		settings->setValue("limit", ui->spinLimit->value());
		settings->setValue("simultaneous", ui->spinSimultaneous->value());
		settings->beginGroup("Customs");
			settings->remove("");
			for (int j = 0; j < m_customNames.size(); j++) {
				settings->setValue(m_customNames[j]->text(), m_customTags[j]->text());
			}
		settings->endGroup();
	settings->endGroup();

	// Resize
	settings->beginGroup("ImageSize");
		settings->setValue("maxWidth", ui->spinResizeMaxWidth->value());
		settings->setValue("maxWidthEnabled", ui->checkResizeMaxWidth->isChecked());
		settings->setValue("maxHeight", ui->spinResizeMaxHeight->value());
		settings->setValue("maxHeightEnabled", ui->checkResizeMaxHeight->isChecked());
	settings->endGroup();

	// Web services
	settings->beginGroup("WebServices");
	for (const ReverseSearchEngine &webService : qAsConst(m_webServices)) {
		settings->beginGroup(QString::number(webService.id()));
		settings->setValue("name", webService.name());
		settings->setValue("url", webService.tpl());
		settings->setValue("order", webService.order());
		settings->endGroup();
	}
	settings->endGroup();

	// Themes
	const QString theme = ui->comboTheme->currentText();
	ThemeLoader themeLoader(savePath("themes/", true));
	if (themeLoader.setTheme(theme)) {
		settings->setValue("theme", theme);
	}

	settings->setValue("Zoom/singleWindow", ui->checkSingleDetailsWindow->isChecked());
	QStringList positions = QStringList() << "top" << "left" << "auto";
	settings->setValue("tagsposition", positions.at(ui->comboTagsPosition->currentIndex()));
	settings->setValue("preload", ui->spinPreload->value());
	settings->setValue("slideshow", ui->spinSlideshow->value());
	settings->setValue("resultsScrollArea", ui->checkResultsScrollArea->isChecked());
	settings->setValue("resultsFixedWidthLayout", ui->checkResultsFixedWidthLayout->isChecked());
	settings->setValue("imageCloseMiddleClick", ui->checkImageCloseMiddleClick->isChecked());
	settings->setValue("imageNavigateScroll", ui->checkImageNavigateScroll->isChecked());
	settings->setValue("Zoom/showTagCount", ui->checkZoomShowTagCount->isChecked());
	settings->setValue("Zoom/viewSamples", ui->checkZoomViewSamples->isChecked());
	QStringList imageTagOrder = QStringList() << "type" << "name" << "count";
	settings->setValue("Zoom/tagOrder", imageTagOrder.at(ui->comboImageTagOrder->currentIndex()));
	QStringList positionsV = QStringList() << "top" << "center" << "bottom";
	QStringList positionsH = QStringList() << "left" << "center" << "right";
	settings->setValue("imagePositionImageV", positionsV.at(ui->comboImagePositionImageV->currentIndex()));
	settings->setValue("imagePositionImageH", positionsH.at(ui->comboImagePositionImageH->currentIndex()));
	settings->setValue("imagePositionAnimationV", positionsV.at(ui->comboImagePositionAnimationV->currentIndex()));
	settings->setValue("imagePositionAnimationH", positionsH.at(ui->comboImagePositionAnimationH->currentIndex()));
	settings->setValue("imagePositionVideoV", positionsV.at(ui->comboImagePositionVideoV->currentIndex()));
	settings->setValue("imagePositionVideoH", positionsH.at(ui->comboImagePositionVideoH->currentIndex()));
	settings->setValue("imageBackgroundColor", ui->lineImageBackgroundColor->text());

	settings->beginGroup("Coloring");
		settings->beginGroup("Colors");
			settings->setValue("artists", ui->lineColoringArtists->text());
			settings->setValue("circles", ui->lineColoringCircles->text());
			settings->setValue("copyrights", ui->lineColoringCopyrights->text());
			settings->setValue("characters", ui->lineColoringCharacters->text());
			settings->setValue("species", ui->lineColoringSpecies->text());
			settings->setValue("metas", ui->lineColoringMetas->text());
			settings->setValue("models", ui->lineColoringModels->text());
			settings->setValue("generals", ui->lineColoringGenerals->text());
			settings->setValue("favorites", ui->lineColoringFavorites->text());
			settings->setValue("keptForLater", ui->lineColoringKeptForLater->text());
			settings->setValue("blacklisteds", ui->lineColoringBlacklisteds->text());
			settings->setValue("ignoreds", ui->lineColoringIgnoreds->text());
		settings->endGroup();
		settings->beginGroup("Fonts");
			settings->setValue("artists", ui->lineColoringArtists->font().toString());
			settings->setValue("circles", ui->lineColoringCircles->font().toString());
			settings->setValue("copyrights", ui->lineColoringCopyrights->font().toString());
			settings->setValue("characters", ui->lineColoringCharacters->font().toString());
			settings->setValue("species", ui->lineColoringSpecies->font().toString());
			settings->setValue("metas", ui->lineColoringMetas->font().toString());
			settings->setValue("models", ui->lineColoringModels->font().toString());
			settings->setValue("generals", ui->lineColoringGenerals->font().toString());
			settings->setValue("favorites", ui->lineColoringFavorites->font().toString());
			settings->setValue("keptForLater", ui->lineColoringKeptForLater->font().toString());
			settings->setValue("blacklisteds", ui->lineColoringBlacklisteds->font().toString());
			settings->setValue("ignoreds", ui->lineColoringIgnoreds->font().toString());
		settings->endGroup();
	settings->endGroup();

	settings->beginGroup("Margins");
		settings->setValue("main", ui->spinMainMargins->value());
		settings->setValue("horizontal", ui->spinHorizontalMargins->value());
		settings->setValue("vertical", ui->spinVerticalMargins->value());
	settings->endGroup();
	settings->setValue("serverBorder", ui->spinServerBorders->value());
	settings->setValue("serverBorderColor", ui->lineBorderColor->text());
	settings->setValue("borders", ui->spinBorders->value());

	settings->beginGroup("Proxy");
		settings->setValue("use", ui->checkProxyUse->isChecked());
		settings->setValue("useSystem", ui->checkProxyUseSystem->isChecked());
		QStringList ptypes = QStringList() << "http" << "socks5";
		settings->setValue("type", ptypes.at(ui->comboProxyType->currentIndex()));
		settings->setValue("hostName", ui->lineProxyHostName->text());
		settings->setValue("port", ui->spinProxyPort->value());
		settings->setValue("user", ui->lineProxyUser->text());
		settings->setValue("password", ui->lineProxyPassword->text());
	settings->endGroup();

	settings->beginGroup("Exec");
		settings->setValue("tag_before", ui->lineCommandsTagAfter->text());
		settings->setValue("image", ui->lineCommandsImage->text());
		settings->setValue("tag_after", ui->lineCommandsTagBefore->text());
		settings->beginGroup("SQL");
			settings->setValue("driver", ui->comboCommandsSqlDriver->currentText());
			settings->setValue("host", ui->lineCommandsSqlHost->text());
			settings->setValue("user", ui->lineCommandsSqlUser->text());
			settings->setValue("password", ui->lineCommandsSqlPassword->text());
			settings->setValue("database", ui->lineCommandsSqlDatabase->text());
			settings->setValue("before", ui->lineCommandsSqlBefore->text());
			settings->setValue("tag_before", ui->lineCommandsSqlTagBefore->text());
			settings->setValue("image", ui->lineCommandsSqlImage->text());
			settings->setValue("tag_after", ui->lineCommandsSqlTagAfter->text());
			settings->setValue("after", ui->lineCommandsSqlAfter->text());
		settings->endGroup();
	settings->endGroup();

	if (settings->value("Proxy/use", false).toBool()) {
		const bool useSystem = settings->value("Proxy/useSystem", false).toBool();
		QNetworkProxyFactory::setUseSystemConfiguration(useSystem);

		if (!useSystem) {
			const QNetworkProxy::ProxyType type = settings->value("Proxy/type", "http") == "http"
				? QNetworkProxy::HttpProxy
				: QNetworkProxy::Socks5Proxy;
			const QNetworkProxy proxy(
				type,
				settings->value("Proxy/hostName").toString(),
				settings->value("Proxy/port").toInt(),
				settings->value("Proxy/user").toString(),
				settings->value("Proxy/password").toString()
			);
			QNetworkProxy::setApplicationProxy(proxy);
			log(QStringLiteral("Enabling application proxy on host \"%1\" and port %2.").arg(settings->value("Proxy/hostName").toString()).arg(settings->value("Proxy/port").toInt()));
		} else {
			log(QStringLiteral("Enabling system-wide proxy."));
		}
	} else if (QNetworkProxy::applicationProxy().type() != QNetworkProxy::NoProxy) {
		QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
		log(QStringLiteral("Disabling application proxy."));
	}

	const QString lang = ui->comboLanguages->currentData().toString();
	if (settings->value("language", "English").toString() != lang) {
		settings->setValue("language", lang);
		emit languageChanged(lang);
	}

	m_profile->sync();
	emit settingsChanged();
}
