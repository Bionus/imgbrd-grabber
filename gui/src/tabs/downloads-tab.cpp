#include "tabs/downloads-tab.h"
#include <QCheckBox>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QShortcut>
#include <QSound>
#include <QTimer>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
	#include <QStorageInfo>
#endif
#include <QtMath>
#include <ui_downloads-tab.h>
#include <algorithm>
#include "batch/add-group-window.h"
#include "batch/add-unique-window.h"
#include "batch/batch-window.h"
#include "batch-download-image.h"
#include "commands/commands.h"
#include "downloader/download-query-group.h"
#include "downloader/download-query-image.h"
#include "downloader/download-query-loader.h"
#include "downloader/image-downloader.h"
#include "functions.h"
#include "helpers.h"
#include "loader/pack-loader.h"
#include "logger.h"
#include "main-window.h"
#include "models/filename.h"
#include "models/page.h"
#include "models/profile.h"


DownloadsTab::DownloadsTab(Profile *profile, DownloadQueue *downloadQueue, MainWindow *parent)
	: QWidget(parent), ui(new Ui::DownloadsTab), m_profile(profile), m_settings(profile->getSettings()), m_downloadQueue(downloadQueue), m_parent(parent), m_getAll(false), m_progressDialog(nullptr), m_batchAutomaticRetries(0)
{
	ui->setupUi(this);

	/*ui->tableBatchGroups->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui->tableBatchUniques->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);*/

	ui->tableBatchGroups->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	ui->tableBatchUniques->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

	QStringList sizes = m_settings->value("batch", "100,100,100,100,100,100,100,100,100").toString().split(',');
	int m = sizes.size() > ui->tableBatchGroups->columnCount() ? ui->tableBatchGroups->columnCount() : sizes.size();
	for (int i = 0; i < m; i++) {
		ui->tableBatchGroups->horizontalHeader()->resizeSection(i, sizes.at(i).toInt());
	}

	QStringList splitterSizes = m_settings->value("batchSplitter", "100,100").toString().split(',');
	ui->splitter->setSizes(QList<int>() << splitterSizes[0].toInt() << splitterSizes[1].toInt());

	QShortcut *actionDeleteBatchGroups = new QShortcut(QKeySequence::Delete, ui->tableBatchGroups);
	actionDeleteBatchGroups->setContext(Qt::WidgetWithChildrenShortcut);
	connect(actionDeleteBatchGroups, &QShortcut::activated, this, &DownloadsTab::batchClearSelGroups);

	QShortcut *actionDeleteBatchUniques = new QShortcut(QKeySequence::Delete, ui->tableBatchUniques);
	actionDeleteBatchUniques->setContext(Qt::WidgetWithChildrenShortcut);
	connect(actionDeleteBatchUniques, &QShortcut::activated, this, &DownloadsTab::batchClearSelUniques);

	connect(m_profile, &Profile::siteDeleted, this, &DownloadsTab::siteDeleted);
}

DownloadsTab::~DownloadsTab()
{
	close();
	delete ui;
}

void DownloadsTab::changeEvent(QEvent *event)
{
	// Automatically re-translate this tab on language change
	if (event->type() == QEvent::LanguageChange) {
		ui->retranslateUi(this);
	}

	QWidget::changeEvent(event);
}

void DownloadsTab::closeEvent(QCloseEvent *event)
{
	Q_UNUSED(event)

	// Columns
	QStringList sizes;
	sizes.reserve(ui->tableBatchGroups->columnCount());
	for (int i = 0; i < ui->tableBatchGroups->columnCount(); i++) {
		sizes.append(QString::number(ui->tableBatchGroups->horizontalHeader()->sectionSize(i)));
	}
	m_settings->setValue("batch", sizes.join(","));

	// Splitter
	QList<int> splitterSizesOrig = ui->splitter->sizes();
	QStringList splitterSizes;
	splitterSizes.reserve(splitterSizesOrig.count());
	for (int size : splitterSizesOrig) {
		splitterSizes.append(QString::number(size));
	}
	m_settings->setValue("batchSplitter", splitterSizes.join(","));
}


void DownloadsTab::siteDeleted(Site *site)
{
	QList<int> batchRows;
	for (int i = 0; i < m_groupBatchs.count(); ++i) {
		const DownloadQueryGroup &batch = m_groupBatchs[i];
		if (batch.site == site) {
			batchRows.append(i);
		}
	}
	batchRemoveGroups(batchRows);

	QList<int> uniquesRows;
	for (int i = 0; i < m_batchs.count(); ++i) {
		const DownloadQueryImage &batch = m_batchs[i];
		if (batch.site == site) {
			uniquesRows.append(i);
		}
	}
	batchRemoveUniques(uniquesRows);
}

void DownloadsTab::batchClear()
{
	// Don't do anything if there's nothing to clear
	if (ui->tableBatchGroups->rowCount() == 0 && ui->tableBatchUniques->rowCount() == 0) {
		return;
	}

	// Confirm deletion
	auto response = QMessageBox::question(this, tr("Confirmation"), tr("Are you sure you want to clear your download list?"), QMessageBox::Yes | QMessageBox::No);
	if (response != QMessageBox::Yes) {
		return;
	}

	m_batchs.clear();
	ui->tableBatchUniques->clearContents();
	ui->tableBatchUniques->setRowCount(0);
	m_groupBatchs.clear();
	ui->tableBatchGroups->clearContents();
	ui->tableBatchGroups->setRowCount(0);
	qDeleteAll(m_progressBars);
	m_progressBars.clear();
	updateGroupCount();
}
void DownloadsTab::batchClearSel()
{
	batchClearSelGroups();
	batchClearSelUniques();
}
void DownloadsTab::batchClearSelGroups()
{
	QList<int> rows;
	for (QTableWidgetItem *selected : ui->tableBatchGroups->selectedItems()) {
		int row = selected->row();
		if (!rows.contains(row)) {
			rows.append(row);
		}
	}

	batchRemoveGroups(rows);
}
void DownloadsTab::batchClearSelUniques()
{
	QList<int> rows;
	for (QTableWidgetItem *selected : ui->tableBatchUniques->selectedItems()) {
		int row = selected->row();
		if (!rows.contains(row)) {
			rows.append(row);
		}
	}

	batchRemoveUniques(rows);
}
void DownloadsTab::batchRemoveGroups(QList<int> rows)
{
	std::sort(rows.begin(), rows.end());

	int rem = 0;
	for (int i : qAsConst(rows)) {
		int pos = i - rem;
		m_progressBars[pos]->deleteLater();
		m_progressBars.removeAt(pos);
		m_groupBatchs.removeAt(pos);
		ui->tableBatchGroups->removeRow(pos);
		rem++;
	}

	updateGroupCount();
}
void DownloadsTab::batchRemoveUniques(QList<int> rows)
{
	std::sort(rows.begin(), rows.end());

	int rem = 0;
	for (int i : qAsConst(rows)) {
		int pos = i - rem;
		ui->tableBatchUniques->removeRow(pos);
		m_batchs.removeAt(pos);
		rem++;
	}

	updateGroupCount();
}

void DownloadsTab::batchMove(int diff)
{
	QList<QTableWidgetItem *> selected = ui->tableBatchGroups->selectedItems();
	if (selected.isEmpty()) {
		return;
	}

	QSet<int> rows;
	for (QTableWidgetItem *item : selected) {
		rows.insert(item->row());
	}

	for (int sourceRow : rows) {
		int destRow = sourceRow + diff;
		if (destRow < 0 || destRow >= ui->tableBatchGroups->rowCount()) {
			return;
		}

		for (int col = 0; col < ui->tableBatchGroups->columnCount(); ++col) {
			QTableWidgetItem *sourceItem = ui->tableBatchGroups->takeItem(sourceRow, col);
			QTableWidgetItem *destItem = ui->tableBatchGroups->takeItem(destRow, col);

			ui->tableBatchGroups->setItem(sourceRow, col, destItem);
			ui->tableBatchGroups->setItem(destRow, col, sourceItem);
		}
	}

	QItemSelection selection;
	for (int i = 0; i < selected.count(); i++) {
		QModelIndex index = ui->tableBatchGroups->model()->index(selected.at(i)->row(), selected.at(i)->column());
		selection.select(index, index);
	}

	auto *selectionModel = new QItemSelectionModel(ui->tableBatchGroups->model(), this);
	selectionModel->select(selection, QItemSelectionModel::ClearAndSelect);
	ui->tableBatchGroups->setSelectionModel(selectionModel);
}
void DownloadsTab::batchMoveUp()
{
	batchMove(-1);
}
void DownloadsTab::batchMoveDown()
{
	batchMove(1);
}

void DownloadsTab::updateBatchGroups(int y, int x)
{
	if (m_allow && x > 0) {
		QString val = ui->tableBatchGroups->item(y, x)->text();
		int toInt = val.toInt();

		m_groupBatchs[y].progressVal = 0;
		m_groupBatchs[y].progressFinished = false;

		switch (x)
		{
			case 1:
				if (m_groupBatchs[y].query.gallery.isNull()) {
					m_groupBatchs[y].query.tags = val.split(' ', QString::SkipEmptyParts);
				}
				break;

			case 3:	m_groupBatchs[y].page = toInt;						break;
			case 6:	m_groupBatchs[y].filename = val;					break;
			case 7:	m_groupBatchs[y].path = val;						break;
			case 8:	m_groupBatchs[y].postFiltering = val.split(' ', QString::SkipEmptyParts);	break;
			case 9:	m_groupBatchs[y].getBlacklisted = (val != "false");	break;
			case 10: m_groupBatchs[y].galleriesCountAsOne = (val != "false");	break;

			case 2:
				if (!m_profile->getSites().contains(val)) {
					error(this, tr("This source is not valid."));
					ui->tableBatchGroups->item(y, x)->setText(m_groupBatchs[y].site->url());
				} else {
					m_groupBatchs[y].site = m_profile->getSites().value(val);
				}
				break;

			case 4:
				if (toInt < 1) {
					error(this, tr("The image per page value must be greater or equal to 1."));
					ui->tableBatchGroups->item(y, x)->setText(QString::number(m_groupBatchs[y].perpage));
				} else {
					m_groupBatchs[y].perpage = toInt;
				}
				break;

			case 5:
				if (toInt < 0) {
					error(this, tr("The image limit must be greater or equal to 0."));
					ui->tableBatchGroups->item(y, x)->setText(QString::number(m_groupBatchs[y].total));
				} else {
					m_groupBatchs[y].total = toInt;
					m_progressBars[y]->setMaximum(toInt);
				}
				break;
		}

		saveLinkList(m_profile->getPath() + "/restore.igl");
	}
}

void DownloadsTab::addGroup()
{
	auto wAddGroup = new AddGroupWindow(m_parent->getSelectedSiteOrDefault(), m_profile, this);
	connect(wAddGroup, &AddGroupWindow::sendData, this, &DownloadsTab::batchAddGroup);
	wAddGroup->show();
}
void DownloadsTab::addUnique()
{
	auto wAddUnique = new AddUniqueWindow(m_parent->getSelectedSiteOrDefault(), m_profile, this);
	connect(wAddUnique, SIGNAL(sendData(DownloadQueryImage)), this, SLOT(batchAddUnique(DownloadQueryImage)));
	wAddUnique->show();
}


void DownloadsTab::batchAddGroup(const DownloadQueryGroup &values)
{
	// Ignore downloads already present in the list
	if (m_groupBatchs.contains(values)) {
		return;
	}

	m_groupBatchs.append(values);
	int pos = m_groupBatchs.count();

	ui->tableBatchGroups->setRowCount(ui->tableBatchGroups->rowCount() + 1);
	int row = ui->tableBatchGroups->rowCount() - 1;
	m_allow = false;

	auto *item = new QTableWidgetItem(getIcon(":/images/status/pending.png"), QString::number(pos));
	item->setFlags(item->flags() ^ Qt::ItemIsEditable);
	ui->tableBatchGroups->setItem(row, 0, item);

	auto *gItem = addTableItem(ui->tableBatchGroups, row, 1, values.query.toString());
	if (!values.query.gallery.isNull()) {
		gItem->setFlags(gItem->flags() & (~Qt::ItemIsEditable));
	}

	addTableItem(ui->tableBatchGroups, row, 2, values.site->url());
	addTableItem(ui->tableBatchGroups, row, 3, QString::number(values.page));
	addTableItem(ui->tableBatchGroups, row, 4, QString::number(values.perpage));
	addTableItem(ui->tableBatchGroups, row, 5, QString::number(values.total));
	addTableItem(ui->tableBatchGroups, row, 6, values.filename);
	addTableItem(ui->tableBatchGroups, row, 7, values.path);
	addTableItem(ui->tableBatchGroups, row, 8, values.postFiltering.join(' '));
	addTableItem(ui->tableBatchGroups, row, 9, values.getBlacklisted ? "true" : "false");
	addTableItem(ui->tableBatchGroups, row, 10, values.galleriesCountAsOne ? "true" : "false");

	auto *progressBar = new QProgressBar(this);
	progressBar->setTextVisible(false);
	progressBar->setMaximum(values.total);
	m_progressBars.append(progressBar);
	ui->tableBatchGroups->setCellWidget(row, 11, progressBar);

	m_allow = true;
	saveLinkList(m_profile->getPath() + "/restore.igl");
	updateGroupCount();
}
void DownloadsTab::updateGroupCount()
{
	int groups = 0;
	for (int i = 0; i < ui->tableBatchGroups->rowCount(); i++) {
		groups += ui->tableBatchGroups->item(i, 5)->text().toInt();
	}
	ui->labelGroups->setText(tr("Groups (%1/%2)").arg(ui->tableBatchGroups->rowCount()).arg(groups));
}
void DownloadsTab::batchAddUnique(const DownloadQueryImage &query, bool save)
{
	// Ignore downloads already present in the list
	if (m_batchs.contains(query)) {
		return;
	}

	log(QStringLiteral("Adding single image: %1").arg(query.image->fileUrl().toString()), Logger::Info);

	m_batchs.append(query);
	ui->tableBatchUniques->setRowCount(ui->tableBatchUniques->rowCount() + 1);

	int row = ui->tableBatchUniques->rowCount() - 1;
	addTableItem(ui->tableBatchUniques, row, 0, QString::number(query.image->id()));
	addTableItem(ui->tableBatchUniques, row, 1, query.image->md5());
	addTableItem(ui->tableBatchUniques, row, 2, query.image->rating());
	addTableItem(ui->tableBatchUniques, row, 3, query.image->tagsString().join(' '));
	addTableItem(ui->tableBatchUniques, row, 4, query.image->fileUrl().toString());
	addTableItem(ui->tableBatchUniques, row, 5, query.image->createdAt().toString(Qt::ISODate));
	addTableItem(ui->tableBatchUniques, row, 6, query.image->search().join(' '));
	addTableItem(ui->tableBatchUniques, row, 7, query.site->name());
	addTableItem(ui->tableBatchUniques, row, 8, query.filename);
	addTableItem(ui->tableBatchUniques, row, 9, query.path);

	if (save) {
		saveLinkList(m_profile->getPath() + "/restore.igl");
	}
}

QTableWidgetItem *DownloadsTab::addTableItem(QTableWidget *table, int row, int col, const QString &text)
{
	auto *item = new QTableWidgetItem(text);
	item->setToolTip(text);

	table->setItem(row, col, item);
	return item;
}


void DownloadsTab::on_buttonSaveLinkList_clicked()
{
	QString lastDir = m_settings->value("linksLastDir", "").toString();
	QString save = QFileDialog::getSaveFileName(this, tr("Save link list"), QDir::toNativeSeparators(lastDir), tr("Imageboard-Grabber links (*.igl)"));
	if (save.isEmpty()) {
		return;
	}

	save = QDir::toNativeSeparators(save);
	m_settings->setValue("linksLastDir", save.section(QDir::separator(), 0, -2));

	if (saveLinkList(save)) {
		QMessageBox::information(this, tr("Save link list"), tr("Link list saved successfully!"));
	} else {
		QMessageBox::critical(this, tr("Save link list"), tr("Error opening file."));
	}
}
bool DownloadsTab::saveLinkList(const QString &filename)
{
	return DownloadQueryLoader::save(filename, m_batchs, m_groupBatchs);
}

void DownloadsTab::on_buttonLoadLinkList_clicked()
{
	QString load = QFileDialog::getOpenFileName(this, tr("Load link list"), QString(), tr("Imageboard-Grabber links (*.igl)"));
	if (load.isEmpty()) {
		return;
	}

	if (loadLinkList(load)) {
		QMessageBox::information(this, tr("Load link list"), tr("Link list loaded successfully!"));
	} else {
		QMessageBox::critical(this, tr("Load link list"), tr("Error opening file."));
	}
}
bool DownloadsTab::loadLinkList(const QString &filename)
{
	QList<DownloadQueryImage> newBatchs;
	QList<DownloadQueryGroup> newGroupBatchs;

	if (!DownloadQueryLoader::load(filename, newBatchs, newGroupBatchs, m_profile)) {
		return false;
	}

	log(tr("Loading %n download(s)", "", newBatchs.count() + newGroupBatchs.count()), Logger::Info);

	m_allow = false;
	for (const auto &queryImage : qAsConst(newBatchs)) {
		batchAddUnique(queryImage, false);
	}
	for (const auto &queryGroup : qAsConst(newGroupBatchs)) {
		ui->tableBatchGroups->setRowCount(ui->tableBatchGroups->rowCount() + 1);

		const int val = queryGroup.progressVal;
		const int max = queryGroup.total;

		int row = ui->tableBatchGroups->rowCount() - 1;

		auto *gItem = addTableItem(ui->tableBatchGroups, row, 1, queryGroup.query.toString());
		if (!queryGroup.query.gallery.isNull()) {
			gItem->setFlags(gItem->flags() & (~Qt::ItemIsEditable));
		}

		addTableItem(ui->tableBatchGroups, row, 2, queryGroup.site->url());
		addTableItem(ui->tableBatchGroups, row, 3, QString::number(queryGroup.page));
		addTableItem(ui->tableBatchGroups, row, 4, QString::number(queryGroup.perpage));
		addTableItem(ui->tableBatchGroups, row, 5, QString::number(queryGroup.total));
		addTableItem(ui->tableBatchGroups, row, 6, queryGroup.filename);
		addTableItem(ui->tableBatchGroups, row, 7, queryGroup.path);
		addTableItem(ui->tableBatchGroups, row, 8, queryGroup.postFiltering.join(' '));
		addTableItem(ui->tableBatchGroups, row, 9, queryGroup.getBlacklisted ? "true" : "false");
		addTableItem(ui->tableBatchGroups, row, 10, queryGroup.galleriesCountAsOne ? "true" : "false");

		m_groupBatchs.append(queryGroup);
		QTableWidgetItem *it = new QTableWidgetItem(getIcon(":/images/status/" + QString(val >= max ? "ok" : (val > 0 ? "downloading" : "pending")) + ".png"), "");
		it->setFlags(it->flags() ^ Qt::ItemIsEditable);
		it->setTextAlignment(Qt::AlignCenter);
		ui->tableBatchGroups->setItem(row, 0, it);

		auto *progressBar = new QProgressBar(this);
		progressBar->setMaximum(queryGroup.total);
		progressBar->setValue(val < 0 || val > max ? 0 : val);
		progressBar->setMinimum(0);
		progressBar->setTextVisible(false);
		m_progressBars.append(progressBar);
		ui->tableBatchGroups->setCellWidget(row, 11, progressBar);
	}
	m_allow = true;
	updateGroupCount();

	return true;
}

QIcon &DownloadsTab::getIcon(const QString &path)
{
	if (!m_icons.contains(path)) {
		m_icons.insert(path, QIcon(path));
	}

	return m_icons[path];
}

bool DownloadsTab::isDownloading() const
{
	return m_getAll;
}


/* Batch download */
void DownloadsTab::batchSel()
{
	getAll(false);
}
void DownloadsTab::getAll(bool all)
{
	// Initial checks
	if (m_getAll) {
		log(QStringLiteral("Batch download start cancelled because another one is already running."), Logger::Warning);
		return;
	}
	if (m_settings->value("Save/path").toString().isEmpty()) {
		error(this, tr("You did not specify a save folder!"));
		return;
	}
	if (m_settings->value("Save/filename").toString().isEmpty()) {
		error(this, tr("You did not specify a filename!"));
		return;
	}
	log(QStringLiteral("Batch download started."), Logger::Info);

	if (m_progressDialog == nullptr) {
		m_progressDialog = new BatchWindow(m_profile->getSettings(), this);
		connect(m_progressDialog, &BatchWindow::paused, this, &DownloadsTab::getAllPause);
		connect(m_progressDialog, &BatchWindow::rejected, this, &DownloadsTab::getAllCancel);
		connect(m_progressDialog, &BatchWindow::skipped, this, &DownloadsTab::getAllSkip);
	}

	// Reinitialize variables
	m_getAll = true;
	ui->widgetDownloadButtons->setDisabled(m_getAll);
	m_getAllDownloaded = 0;
	m_getAllExists = 0;
	m_getAllIgnored = 0;
	m_getAllIgnoredPre = 0;
	m_getAll404s = 0;
	m_getAllErrors = 0;
	m_getAllSkipped = 0;
	m_getAllResumed = 0;
	m_getAllRemaining.clear();
	m_getAllFailed.clear();
	m_getAllDownloading.clear();
	m_getAllSkippedImages.clear();
	m_batchPending.clear();
	m_waitingPackLoaders.clear();
	m_currentPackLoader = nullptr;
	m_batchUniqueDownloading.clear();

	if (!all) {
		QList<int> tdl;
		for (QTableWidgetItem *item : ui->tableBatchUniques->selectedItems()) {
			int row = item->row();
			if (tdl.contains(row)) {
				continue;
			}
			tdl.append(row);

			DownloadQueryImage batch = m_batchs[row];
			BatchDownloadImage d;
			d.image = batch.image;
			d.queryImage = &batch;

			m_getAllRemaining.append(d);
			m_batchUniqueDownloading.insert(row);
		}
	} else {
		for (int j = 0; j < m_batchs.count(); ++j) {
			const DownloadQueryImage &batch = m_batchs[j];
			if (batch.image->fileUrl().isEmpty()) {
				log(QStringLiteral("No file URL provided in image download query"), Logger::Warning);
				continue;
			}

			BatchDownloadImage d;
			d.image = batch.image;
			d.queryImage = &batch;

			m_getAllRemaining.append(d);
			m_batchUniqueDownloading.insert(j);
		}
	}
	m_getAllLimit = m_batchs.size();

	m_allow = false;
	for (int i = 0; i < ui->tableBatchGroups->rowCount(); i++) {
		ui->tableBatchGroups->item(i, 0)->setIcon(getIcon(":/images/status/pending.png"));
	}
	m_allow = true;
	m_profile->getCommands().before();
	m_batchDownloading.clear();

	QSet<int> toDownload = QSet<int>();
	for (QTableWidgetItem *item : ui->tableBatchGroups->selectedItems()) {
		if (!toDownload.contains(item->row())) {
			toDownload.insert(item->row());
		}
	}

	int resumeCount = 0;
	if (all || !toDownload.isEmpty()) {
		for (int j = 0; j < m_groupBatchs.count(); ++j) {
			if (all || toDownload.contains(j)) {
				if (m_progressBars.length() > j && m_progressBars[j] != nullptr) {
					m_progressBars[j]->setValue(0);
					m_progressBars[j]->setMinimum(0);
					// m_progressBars[j]->setMaximum(100);
				}

				DownloadQueryGroup b = m_groupBatchs[j];
				m_batchPending.insert(j, b);
				m_getAllLimit += b.total;
				m_batchDownloading.insert(j);

				if (b.progressVal > 0 && !b.progressFinished) {
					resumeCount += b.progressVal;
				}
			}
		}
	}

	// Try to resume downloads that were stopped in the middle
	bool resume = resumeCount > 0;
	if (resume) {
		int resumeAnswer = QMessageBox::question(this, "", "Some downloads were started but not finished. Do you want to continue from where you left off?");
		if (resumeAnswer == QMessageBox::Yes) {
			m_getAllResumed = resumeCount;
		} else {
			resume = false;
		}
	}
	for (const int b : m_batchDownloading) {
		if (m_groupBatchs[b].progressFinished || !resume) {
			m_groupBatchs[b].progressVal = 0;
			m_batchPending[b].progressVal = 0;
		}
		m_groupBatchs[b].progressFinished = false;
		m_batchPending[b].progressFinished = false;
	}

	// Confirm before downloading possibly more than 10,000 images
	bool tooBig = false;
	if (m_getAllLimit > 10000 && m_settings->value("confirm_big_downloads", true).toBool()) {
		QMessageBox msgBox(this);
		msgBox.setText(tr("You are going to download up to %1 images, which can take a long time and space on your computer. Are you sure you want to proceed?").arg(m_getAllLimit));
		msgBox.setIcon(QMessageBox::Warning);
		QCheckBox dontAskCheckBox(tr("Don't ask me again"));
		dontAskCheckBox.setCheckable(true);
		#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
			msgBox.setCheckBox(&dontAskCheckBox);
		#else
			msgBox.addButton(&dontShowCheckBox, QMessageBox::ResetRole);
		#endif
		msgBox.addButton(QMessageBox::Yes);
		msgBox.addButton(QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Cancel);
		int response = msgBox.exec();

		// Don't close on "cancel"
		if (response != QMessageBox::Yes) {
			tooBig = true;
		}

		// Remember checkbox
		else if (dontAskCheckBox.checkState() == Qt::Checked) {
			m_settings->setValue("confirm_big_downloads", false);
		}
	}

	if (tooBig || (m_batchPending.isEmpty() && m_getAllRemaining.isEmpty())) {
		log(tooBig ? QStringLiteral("Batch download too big") : QStringLiteral("Nothing to download"), Logger::Info);
		m_getAll = false;
		ui->widgetDownloadButtons->setEnabled(true);
		return;
	}

	m_progressDialog->show();
	getAllLogin();
}

void DownloadsTab::getAllLogin()
{
	m_progressDialog->clear();
	m_progressDialog->setText(tr("Logging in, please wait..."));

	m_getAllLogins.clear();
	for (auto it = m_batchPending.constBegin(); it != m_batchPending.constEnd(); ++it) {
		Site *site = it.value().site;
		if (!m_getAllLogins.contains(site)) {
			m_getAllLogins.append(site);
		}
	}

	if (m_getAllLogins.empty()) {
		getAllFinishedLogins();
		return;
	}

	m_progressDialog->setCurrentValue(0);
	m_progressDialog->setCurrentMax(m_getAllLogins.count());

	for (Site *site : m_getAllLogins) {
		connect(site, &Site::loggedIn, this, &DownloadsTab::getAllFinishedLogin, Qt::QueuedConnection);
		site->login();
	}
}
void DownloadsTab::getAllFinishedLogin(Site *site, Site::LoginResult result)
{
	Q_UNUSED(result)

	if (m_getAllLogins.empty()) {
		return;
	}

	m_progressDialog->setCurrentValue(m_progressDialog->currentValue() + 1);
	m_getAllLogins.removeAll(site);

	if (m_getAllLogins.empty()) {
		getAllFinishedLogins();
	}
}

void DownloadsTab::getAllFinishedLogins()
{
	bool usePacking = m_settings->value("packing_enable", true).toBool();
	int imagesPerPack = m_settings->value("packing_size", 1000).toInt();

	int total = 0;
	for (auto it = m_batchPending.constBegin(); it != m_batchPending.constEnd(); ++it) {
		DownloadQueryGroup b = it.value();
		total += b.total;

		auto packLoader = new PackLoader(m_profile, b, usePacking ? imagesPerPack : -1, this);
		connect(packLoader, &PackLoader::finishedPage, this, &DownloadsTab::getAllFinishedPage);
		m_waitingPackLoaders.enqueue(packLoader);
	}

	m_getAllImagesCount = total;
	getNextPack();
}

void DownloadsTab::getNextPack()
{
	// If the current pack loader is not finished
	if (m_currentPackLoader != nullptr && m_currentPackLoader->hasNext()) {
		getAllGetPages();
	}
	// If there are pending packs
	else if (!m_waitingPackLoaders.isEmpty()) {
		if (m_currentPackLoader != nullptr) {
			m_currentPackLoader->deleteLater();
		}

		m_currentPackLoader = m_waitingPackLoaders.dequeue();
		m_currentPackLoader->start();

		getAllGetPages();
	}
	// Only images to download
	else {
		m_batchAutomaticRetries = m_settings->value("Save/automaticretries", 0).toInt();
		getAllImages();
	}
}

void DownloadsTab::getAllGetPages()
{
	m_progressDialog->clearImages();
	m_progressDialog->setText(tr("Downloading pages, please wait..."));

	const auto &query = m_currentPackLoader->query();
	const int images = m_currentPackLoader->nextPackSize();
	const int pages = qMax(1, qCeil(static_cast<qreal>(images) / query.perpage));

	m_progressDialog->setCurrentValue(0);
	m_progressDialog->setCurrentMax(pages);
	m_batchCurrentPackSize = images;

	getAllFinishedImages(m_currentPackLoader->next());
}

/**
 * Called when a page have been loaded and parsed.
 *
 * @param page The loaded page
 */
void DownloadsTab::getAllFinishedPage(Page *page)
{
	Q_UNUSED(page)

	m_progressDialog->setCurrentValue(m_progressDialog->currentValue() + 1);
}

/**
 * Called when a page have been loaded and parsed.
 *
 * @param images The images results on this page
 */
void DownloadsTab::getAllFinishedImages(const QList<QSharedPointer<Image>> &images)
{
	int row = -1;
	for (auto it = m_batchPending.constBegin(); it != m_batchPending.constEnd(); ++it) {
		if (it.value() == m_currentPackLoader->query()) {
			row = it.key();
			break;
		}
	}
	if (row < 0) {
		log("Images received from unknown batch", Logger::Error);
		return;
	}

	for (const auto &img : images) {
		BatchDownloadImage d;
		d.image = img;
		d.queryGroup = &m_batchPending[row];
		m_getAllRemaining.append(d);
	}

	// Ignore for aborted/resumed calls (partial packs)
	if (m_getAll && !images.isEmpty()) {
		m_progressBars[row]->setValue(0);
		m_progressBars[row]->setMaximum(images.count());

		// Update image to take into account unlisted images
		int unlisted = m_batchCurrentPackSize - images.count();
		m_getAllImagesCount -= unlisted;
	}

	// Stop here if we're paused
	if (m_getAll) {
		m_batchAutomaticRetries = m_settings->value("Save/automaticretries", 0).toInt();
		getAllImages();
	}
}

/**
 * Called when all pages have been loaded and parsed from all sources.
 */
void DownloadsTab::getAllImages()
{
	log(QStringLiteral("All images' urls have been received (%1).").arg(m_getAllRemaining.count()), Logger::Info);

	// We add the images to the download dialog
	m_progressDialog->clearImages();
	m_progressDialog->setText(tr("Preparing images, please wait..."));
	m_progressDialog->setCount(m_getAllRemaining.count());
	for (const BatchDownloadImage &download : qAsConst(m_getAllRemaining)) {
		const int siteId = download.siteId(m_groupBatchs);
		QSharedPointer<Image> img = download.image;

		// We add the image
		m_progressDialog->addImage(img->url(), siteId, img->fileSize());
		connect(img.data(), &Image::urlChanged, m_progressDialog, &BatchWindow::imageUrlChanged);
		connect(img.data(), &Image::urlChanged, this, &DownloadsTab::imageUrlChanged);
	}

	// Set some values on the batch window
	m_progressDialog->updateColumns();
	m_progressDialog->setText(tr("Downloading images..."));
	m_progressDialog->setCurrentValue(0);
	m_progressDialog->setCurrentMax(m_getAllRemaining.count());
	m_progressDialog->setTotalValue(m_getAllDownloaded + m_getAllExists + m_getAllIgnored + m_getAllErrors + m_getAllResumed);
	m_progressDialog->setTotalMax(m_getAllImagesCount);

	// We start the simultaneous downloads
	int count = qMax(1, qMin(m_settings->value("Save/simultaneous").toInt(), 10));
	m_getAllCurrentlyProcessing.store(count);
	for (int i = 0; i < count; i++) {
		_getAll();
	}
}

void DownloadsTab::_getAll()
{
	// We quit as soon as the user cancels
	if (m_progressDialog->cancelled()) {
		return;
	}

	// If there are still images do download
	if (!m_getAllRemaining.empty()) {
		// We take the first image to download
		BatchDownloadImage download = m_getAllRemaining.takeFirst();
		m_getAllDownloading.append(download);

		int siteId = download.siteId(m_groupBatchs);
		getAllGetImage(download, siteId);
	}
	// When the batch download finishes
	else if (m_getAllCurrentlyProcessing.fetchAndAddRelaxed(-1) == 1 && m_getAll) {
		getAllFinished();
	}
}

void DownloadsTab::getAllImageOk(const BatchDownloadImage &download, int siteId, bool retry)
{
	m_downloadTime.remove(download.image->url());
	m_downloadTimeLast.remove(download.image->url());

	if (retry) {
		return;
	}

	m_progressDialog->setCurrentValue(m_progressDialog->currentValue() + 1);
	m_progressDialog->setTotalValue(m_getAllDownloaded + m_getAllExists + m_getAllIgnored + m_getAllErrors + m_getAllResumed);

	if (siteId >= 0) {
		int row = getRowForSite(siteId);
		m_progressBars[siteId - 1]->setValue(m_progressBars[siteId - 1]->value() + 1);
		if (m_progressBars[siteId - 1]->value() >= m_progressBars[siteId - 1]->maximum()) {
			ui->tableBatchGroups->item(row, 0)->setIcon(getIcon(":/images/status/ok.png"));
		}

		m_groupBatchs[siteId - 1].progressVal++;
		m_batchPending[siteId - 1].progressVal++;
	}

	m_getAllDownloading.removeAll(download);
	QCoreApplication::processEvents();
	QTimer::singleShot(0, this, SLOT(_getAll()));
}

void DownloadsTab::imageUrlChanged(const QUrl &before, const QUrl &after)
{
	m_downloadTimeLast.insert(after, m_downloadTimeLast[before]);
	m_downloadTimeLast.remove(before);
	m_downloadTime.insert(after, m_downloadTime[before]);
	m_downloadTime.remove(before);
}
void DownloadsTab::getAllProgress(const QSharedPointer<Image> &img, qint64 bytesReceived, qint64 bytesTotal)
{
	const QUrl url = img->url();

	if (!m_downloadTimeLast.contains(url)) {
		return;
	}

	if (m_downloadTimeLast[url].elapsed() >= 200 && bytesReceived > 0) {
		m_downloadTimeLast[url].restart();
		const int elapsed = m_downloadTime[url].elapsed();
		const double speed = elapsed != 0 ? (bytesReceived * 1000) / elapsed : 0;
		m_progressDialog->speedImage(url, speed);
	}

	int percent = 0;
	if (bytesTotal > 0) {
		const qreal pct = static_cast<qreal>(bytesReceived) / static_cast<qreal>(bytesTotal);
		percent = qFloor(pct * 100);
	}

	m_progressDialog->sizeImage(url, bytesTotal);
	m_progressDialog->statusImage(url, percent);
}

int DownloadsTab::getRowForSite(int siteId)
{
	return siteId - 1;
}

void DownloadsTab::getAllGetImage(const BatchDownloadImage &download, int siteId)
{
	QSharedPointer<Image> img = download.image;

	// If there is already a downloader for this image, we simply restart it
	if (m_getAllImageDownloaders.contains(img)) {
		m_getAllImageDownloaders[img]->save();
		return;
	}

	// Stop here if we're paused
	if (!m_getAll) {
		return;
	}

	// Row
	int row = getRowForSite(siteId);

	// Path
	QString filename = download.query()->filename;
	QString path = download.query()->path;
	if (siteId >= 0) {
		ui->tableBatchGroups->item(row, 0)->setIcon(getIcon(":/images/status/downloading.png"));
	}

	// Track download progress
	m_progressDialog->loadingImage(img->url());
	m_downloadTime.insert(img->url(), QTime());
	m_downloadTime[img->url()].start();
	m_downloadTimeLast.insert(img->url(), QTime());
	m_downloadTimeLast[img->url()].start();

	// Start loading and saving image
	log(QStringLiteral("Loading image from `%1` %2").arg(img->fileUrl().toString()).arg(m_getAllDownloading.size()), Logger::Info);
	int count = m_getAllDownloaded + m_getAllExists + m_getAllIgnored + m_getAllErrors + m_getAllResumed + 1;
	bool getBlacklisted = download.queryGroup == nullptr || download.queryGroup->getBlacklisted;
	auto imgDownloader = new ImageDownloader(m_profile, img, filename, path, count, true, false, this);
	if (!getBlacklisted) {
		imgDownloader->setBlacklist(&m_profile->getBlacklist());
	}
	connect(imgDownloader, &ImageDownloader::saved, this, &DownloadsTab::getAllGetImageSaved, Qt::UniqueConnection);
	connect(imgDownloader, &ImageDownloader::downloadProgress, this, &DownloadsTab::getAllProgress, Qt::UniqueConnection);
	m_getAllImageDownloaders[img] = imgDownloader;
	imgDownloader->save();
}

void DownloadsTab::getAllGetImageSaved(const QSharedPointer<Image> &img, QList<ImageSaveResult> result)
{
	// Delete ImageDownloader to prevent leaks
	m_getAllImageDownloaders[img]->deleteLater();
	m_getAllImageDownloaders.remove(img);

	// Find related download query
	const BatchDownloadImage *downloadPtr = nullptr;
	for (const BatchDownloadImage &i : qAsConst(m_getAllDownloading)) {
		if (i.image == img) {
			downloadPtr = &i;
		}
	}
	if (downloadPtr == nullptr) {
		log(QStringLiteral("Saved image signal received from unknown sender"), Logger::Error);
		return;
	}
	BatchDownloadImage download = *downloadPtr;

	// Save error count to compare it later on
	bool diskError = false;
	const auto res = result.first().result;

	// Disk writing errors
	for (const ImageSaveResult &re : result) {
		if (re.result == Image::SaveResult::Error) {
			diskError = true;

			if (!m_progressDialog->isPaused()) {
				m_progressDialog->pause();

				bool isDriveFull;
				QString drive;
				#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
					QDir destinationDir = QFileInfo(re.path).absoluteDir();
					QStorageInfo storage(destinationDir);
					isDriveFull = storage.isValid() && (storage.bytesAvailable() < img->fileSize() || storage.bytesAvailable() < 20 * 1024 * 1024);
					QString rootPath = storage.rootPath();
					#ifdef Q_OS_WIN
						drive = QStringLiteral("%1 (%2)").arg(storage.name(), rootPath.endsWith("/") ? rootPath.left(rootPath.length() - 1) : rootPath);
					#else
						drive = rootPath;
					#endif
				#else
					isDriveFull = false;
				#endif

				QString msg;
				if (isDriveFull) {
					msg = tr("Not enough space on the destination drive \"%1\".\nPlease free some space before resuming the download.").arg(drive);
				} else {
					msg = tr("An error occured saving the image.\n%1\nPlease solve the issue before resuming the download.").arg(re.path);
				}
				QMessageBox::critical(m_progressDialog, tr("Error"), msg);
			}
		}
	}

	if (res == Image::SaveResult::NetworkError) {
		m_getAllErrors++;
		m_getAllFailed.append(download);
	} else if (res == Image::SaveResult::NotFound) {
		m_getAll404s++;
	} else if (res == Image::SaveResult::AlreadyExistsDisk) {
		m_getAllExists++;
	} else if (res == Image::SaveResult::Blacklisted || res == Image::SaveResult::AlreadyExistsMd5) {
		m_getAllIgnored++;
	} else if (!diskError) {
		m_getAllDownloaded++;
	}

	m_progressDialog->loadedImage(img->url(), res);

	int siteId = download.siteId(m_groupBatchs);
	getAllImageOk(download, siteId, diskError);
}

void DownloadsTab::getAllCancel()
{
	log(QStringLiteral("Cancelling downloads..."), Logger::Info);
	m_progressDialog->cancel();
	if (m_currentPackLoader != nullptr) {
		m_currentPackLoader->abort();
	}
	for (auto it = m_getAllImageDownloaders.constBegin(); it != m_getAllImageDownloaders.constEnd(); ++it) {
		it.value()->abort();
	}
	m_getAll = false;
	ui->widgetDownloadButtons->setEnabled(true);
	DONE();
}

void DownloadsTab::getAllSkip()
{
	log(QStringLiteral("Skipping downloads..."), Logger::Info);

	int count = m_getAllDownloading.count();
	for (auto it = m_getAllImageDownloaders.constBegin(); it != m_getAllImageDownloaders.constEnd(); ++it) {
		it.value()->abort();
	}
	m_getAllSkippedImages.append(m_getAllDownloading);
	m_getAllDownloading.clear();

	m_getAllSkipped += count;
	m_progressDialog->setTotalValue(m_getAllDownloaded + m_getAllExists + m_getAllIgnored + m_getAllErrors + m_getAllResumed);
	m_getAllCurrentlyProcessing.store(count);
	for (int i = 0; i < count; ++i) {
		_getAll();
	}

	DONE();
}

void DownloadsTab::getAllFinished()
{
	if (!m_waitingPackLoaders.isEmpty() || (m_currentPackLoader != nullptr && m_currentPackLoader->hasNext())) {
		getNextPack();
		return;
	}

	log(QStringLiteral("Images download finished."), Logger::Info);
	m_progressDialog->setTotalValue(m_progressDialog->totalMax());

	// Delete objects
	if (m_currentPackLoader != nullptr) {
		m_currentPackLoader->deleteLater();
		m_currentPackLoader = nullptr;
	}

	// Retry in case of error
	int failedCount = m_getAllErrors + m_getAllSkipped;
	if (failedCount > 0) {
		int response;
		if (m_batchAutomaticRetries > 0) {
			m_batchAutomaticRetries--;
			response = QMessageBox::Yes;
		} else {
			// Trigger minor end actions on retry
			switch (m_progressDialog->endAction())
			{
				case 2: openTray();                             break;
				case 4: QSound::play(":/sounds/finished.wav");  break;
			}
			activateWindow();

			int totalCount = m_getAllDownloaded + m_getAllIgnored + m_getAllExists + m_getAll404s + m_getAllErrors + m_getAllSkipped + m_getAllResumed;
			response = QMessageBox::question(this, tr("Getting images"), tr("Errors occured during the images download. Do you want to restart the download of those images? (%1/%2)").arg(failedCount).arg(totalCount), QMessageBox::Yes | QMessageBox::No);
		}

		if (response == QMessageBox::Yes) {
			m_getAll = true;
			m_progressDialog->clear();
			m_getAllRemaining.clear();
			m_getAllRemaining.append(m_getAllFailed);
			m_getAllRemaining.append(m_getAllSkippedImages);
			m_getAllImagesCount = m_getAllRemaining.count();
			m_getAllFailed.clear();
			m_getAllSkippedImages.clear();
			m_getAllDownloaded = 0;
			m_getAllExists = 0;
			m_getAllIgnored = 0;
			m_getAllIgnoredPre = 0;
			m_getAll404s = 0;
			m_getAllErrors = 0;
			m_getAllSkipped = 0;
			m_getAllResumed = 0;
			m_progressDialog->show();
			getAllImages();
			return;
		}
	}

	// Download result
	QMessageBox::information(
		this,
		tr("Getting images"),
		QString(
			tr("%n file(s) downloaded successfully.", "", m_getAllDownloaded) + "\r\n" +
			tr("%n file(s) ignored.", "", m_getAllIgnored + m_getAllIgnoredPre) + "\r\n" +
			tr("%n file(s) already existing.", "", m_getAllExists) + "\r\n" +
			tr("%n file(s) not found on the server.", "", m_getAll404s) + "\r\n" +
			tr("%n file(s) skipped.", "", m_getAllSkipped) + "\r\n" +
			tr("%n file(s) skipped from a previous download.", "", m_getAllResumed) + "\r\n" +
			tr("%n error(s).", "", m_getAllErrors)
		)
	);

	// Mark downloads as finished
	for (const int b : m_batchDownloading) {
		m_groupBatchs[b].progressFinished = true;
	}

	// Final action
	switch (m_progressDialog->endAction())
	{
		case 1: m_progressDialog->close();              break;
		case 2: openTray();                             break;
		case 3: m_parent->saveFolder();                 break;
		case 4: QSound::play(":/sounds/finished.wav");  break;
		case 5: shutDown();                             break;
	}
	activateWindow();
	m_getAll = false;

	// Remove after download and retries are finished
	if (m_progressDialog->endRemove()) {
		batchRemoveGroups(m_batchDownloading.toList());
		batchRemoveUniques(m_batchUniqueDownloading.toList());
	}

	// End of batch download
	m_profile->getCommands().after();
	ui->widgetDownloadButtons->setEnabled(true);
	log(QStringLiteral("Batch download finished"), Logger::Info);
}

void DownloadsTab::getAllPause()
{
	if (m_progressDialog->isPaused()) {
		log(QStringLiteral("Pausing downloads..."), Logger::Info);
		m_getAll = false;
		if (m_currentPackLoader != nullptr) {
			m_currentPackLoader->abort();
		}
		for (auto it = m_getAllImageDownloaders.constBegin(); it != m_getAllImageDownloaders.constEnd(); ++it) {
			it.value()->abort();
		}
	} else {
		log(QStringLiteral("Recovery of downloads..."), Logger::Info);
		m_getAll = true;
		if (m_getAllDownloading.isEmpty()) {
			getAllFinishedImages(QList<QSharedPointer<Image>>());
		} else {
			for (const auto &download : qAsConst(m_getAllDownloading)) {
				getAllGetImage(download, download.siteId(m_groupBatchs));
			}
		}
	}
	DONE();
}
