#include "sources/sources-window.h"
#include <QCloseEvent>
#include <QDesktopServices>
#include <QInputDialog>
#include <ui_sources-window.h>
#include "functions.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"
#include "network/network-manager.h"
#include "network/network-reply.h"
#include "sources/site-window.h"
#include "sources/sources-settings-window.h"
#include "ui/QAffiche.h"
#include "ui/QBouton.h"


SourcesWindow::SourcesWindow(Profile *profile, QList<Site*> selected, QWidget *parent)
	: QDialog(parent), ui(new Ui::SourcesWindow), m_profile(profile), m_selected(std::move(selected)), m_sites(profile->getSites()), m_sources(profile->getSources()), m_checkForSourceReply(nullptr)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	restoreGeometry(m_profile->getSettings()->value("Sources/geometry").toByteArray());

	addCheckboxes();

	ui->gridLayout->setColumnStretch(0, 1);
	connect(ui->checkBox, &QCheckBox::clicked, this, &SourcesWindow::checkClicked);
	checkUpdate();

	// Presets
	m_presets = loadPresets(m_profile->getSettings());
	showPresets();

	// Check for updates in the model files
	checkForUpdates();

	// Check if there is any reported problem for one of the sources
	checkForSourceIssues();

	ui->buttonBox->setFocus();
}
SourcesWindow::~SourcesWindow()
{
	delete ui;
}

/**
 * When closed, the window emit a signal which will trigger a slot in the main window.
 * @param	event	The event triggered wy window's closing
 * @todo	Why use a signal, since we can simply use a pointer to the parent window, or a signal giving not a pointer to this window, but directly m_selected ?
 */
void SourcesWindow::closeEvent(QCloseEvent *event)
{
	QSettings *settings = m_profile->getSettings();
	savePresets(settings);
	settings->setValue("Sources/geometry", saveGeometry());

	emit closed();
	event->accept();
}

/**
 * Update the "Check all" checkbox according to checked checkboxes.
 */
void SourcesWindow::checkUpdate()
{
	bool oneChecked = false;
	bool oneUnchecked = false;
	for (const auto &row : qAsConst(m_rows)) {
		if (row.check->isChecked()) {
			oneChecked = true;
		} else {
			oneUnchecked = true;
		}
	}
	if (oneChecked && !oneUnchecked) {
		ui->checkBox->setCheckState(Qt::Checked);
	} else if (!oneChecked && oneUnchecked) {
		ui->checkBox->setCheckState(Qt::Unchecked);
	} else {
		ui->checkBox->setCheckState(Qt::PartiallyChecked);
	}

	// Update preset save button
	if (ui->comboPresets->currentIndex() > 0) {
		ui->buttonPresetSave->setEnabled(true);
	}
}

/**
 * Alternate between the checked and unchecked state of the tri-state checkbox "Check all".
 */
void SourcesWindow::checkClicked()
{
	if (ui->checkBox->checkState() != Qt::Unchecked) {
		ui->checkBox->setCheckState(Qt::Checked);
	}
	checkAll(ui->checkBox->checkState());
}

/**
 * Saves current selection then close the window.
 */
void SourcesWindow::valid()
{
	emit valid(selected());
	close();
}

void SourcesWindow::openSite(const QString &site) const
{
	QDesktopServices::openUrl(m_sites.value(site)->fixUrl("/"));
}

void SourcesWindow::settingsSite(const QString &site)
{
	SourcesSettingsWindow *ssw = new SourcesSettingsWindow(m_profile, m_sites.value(site), this);
	connect(ssw, &SourcesSettingsWindow::siteDeleted, this, &SourcesWindow::deleteSite);
	ssw->show();
}

/**
 * Delete a site from the sources list.
 * @param	site	The url of the site to delete.
 */
void SourcesWindow::deleteSite(const QString &site)
{
	int index = -1;
	for (int i = 0; i < m_rows.count(); ++i) {
		const auto &row = m_rows[i];
		if (row.site->url() != site) {
			continue;
		}

		row.check->hide();
		ui->gridLayout->removeWidget(row.check);
		row.check->deleteLater();

		row.button->hide();
		ui->gridLayout->removeWidget(row.button);
		row.button->deleteLater();

		for (QLabel *label : qAsConst(row.labels)) {
			label->hide();
			ui->gridLayout->removeWidget(label);
			label->deleteLater();
		}

		m_profile->removeSite(row.site);
		m_selected.removeAll(row.site);

		index = i;
	}

	if (index != -1) {
		m_rows.removeAt(index);
		m_siteRows.remove(site);
	}
}

/**
 * Open the window to add a site.
 */
void SourcesWindow::addSite()
{
	auto *sw = new SiteWindow(m_profile, this);
	connect(sw, &SiteWindow::accepted, this, &SourcesWindow::updateCheckboxes);
	sw->show();
}

void SourcesWindow::updateCheckboxes()
{
	removeCheckboxes();
	addCheckboxes();
}

void SourcesWindow::removeCheckboxes()
{
	for (auto &row : qAsConst(m_rows)) {
		ui->gridLayout->removeWidget(row.check);
		row.check->deleteLater();

		ui->gridLayout->removeWidget(row.button);
		row.button->deleteLater();

		for (QLabel *label : qAsConst(row.labels)) {
			ui->gridLayout->removeWidget(label);
			label->deleteLater();
		}
	}

	m_rows.clear();
}

/**
 * Add a site to the list.
 */
void SourcesWindow::addCheckboxes()
{
	QString t = m_profile->getSettings()->value("Sources/Types", "icon").toString();

	int i = 0;
	for (auto it = m_sites.constBegin(); it != m_sites.constEnd(); ++it) {
		Site *site = it.value();

		SourceRow row;
		row.site = site;

		auto *check = new QCheckBox(this);
			check->setChecked(m_selected.contains(site));
			check->setText(site->url());
			connect(check, SIGNAL(stateChanged(int)), this, SLOT(checkUpdate()));
			row.check = check;
			ui->gridLayout->addWidget(check, i, 0);

		int n = 1;
		if (t != "hide") {
			if (t == "icon" || t == "both") {
				QAffiche *image = new QAffiche(it.key(), 0, QColor(), this);
				image->setPixmap(QPixmap(site->getSource()->getPath() + "/icon.png").scaled(QSize(16, 16)));
				image->setCursor(Qt::PointingHandCursor);
				connect(image, SIGNAL(clicked(QString)), this, SLOT(openSite(QString)));
				ui->gridLayout->addWidget(image, i, n);
				row.labels.append(image);
				n++;
			}
			if (t == "text" || t == "both") {
				QLabel *type = new QLabel(site->getSource()->getName(), this);
				ui->gridLayout->addWidget(type, i, n);
				row.labels.append(type);
				n++;
			}
		}

		QBouton *del = new QBouton(it.key());
			del->setParent(this);
			del->setText(tr("Options"));
			connect(del, SIGNAL(appui(QString)), this, SLOT(settingsSite(QString)));
			row.button = del;
			ui->gridLayout->addWidget(del, i, n);

		m_rows.append(row);
		m_siteRows.insert(site->url(), i);

		i++;
	}

	/*int n =  0+(t == "icon" || t == "both")+(t == "text" || t == "both");
	for (int i = 0; i < m_checks.count(); i++) {
		ui->gridLayout->addWidget(m_checks.at(i), i, 0);
		m_checks.at(i)->show();
		if (!m_labels.isEmpty()) {
			for (int r = 0; r < n; r++) {
				ui->gridLayout->addWidget(m_labels.at(i*n+r), i*n+r, 1);
				m_labels.at(i*n+r)->show();
			}
		}
		ui->gridLayout->addWidget(m_buttons.at(i), i, n+1);
		m_buttons.at(i)->show();
	}*/
}

/**
 * Check of uncheck all checkboxes, according to "check".
 *
 * @param	check	Qt::CheckState saying if we must check or uncheck everything (0 = uncheck, 2 = check)
 */
void SourcesWindow::checkAll(int check)
{
	for (const auto &row : qAsConst(m_rows)) {
		row.check->setChecked(check == 2);
	}
}

void SourcesWindow::checkForUpdates()
{
	for (auto it = m_sources.constBegin(); it != m_sources.constEnd(); ++it) {
		const SourceUpdater &updater = it.value()->getUpdater();
		connect(&updater, &SourceUpdater::finished, this, &SourcesWindow::checkForUpdatesReceived);
		updater.checkForUpdates();
	}
}
void SourcesWindow::checkForUpdatesReceived(const QString &sourceName, bool isNew)
{
	if (!isNew) {
		return;
	}

	Source *source = m_sources[sourceName];
	for (Site *site : source->getSites()) {
		if (!m_siteRows.contains(site->url())) {
			continue;
		}

		int pos = m_siteRows.value(site->url());
		m_rows[pos].labels[0]->setPixmap(QPixmap(":/images/icons/update.png"));
		m_rows[pos].labels[0]->setToolTip(tr("An update for this source is available."));
	}
}

void SourcesWindow::checkForSourceIssues()
{
	auto *accessManager = new NetworkManager(this);
	m_checkForSourceReply = accessManager->get(QNetworkRequest(QUrl(SOURCE_ISSUES_URL)));

	connect(m_checkForSourceReply, &NetworkReply::finished, this, &SourcesWindow::checkForSourceIssuesReceived);
}
void SourcesWindow::checkForSourceIssuesReceived()
{
	if (m_checkForSourceReply->error() != NetworkReply::NetworkError::NoError) {
		return;
	}

	QString source = m_checkForSourceReply->readAll();
	QStringList issues = source.split("\n");

	for (const QString &issue : issues) {
		const int index = issue.indexOf(':');
		if (issue.isEmpty() || index < 0) {
			return;
		}

		const QString &site = issue.left(index).trimmed();
		const QString &desc = issue.mid(index + 1).trimmed();

		if (!m_siteRows.contains(site)) {
			continue;
		}

		int pos = m_siteRows.value(site);
		m_rows[pos].labels[0]->setPixmap(QPixmap(":/images/icons/warning.png"));
		m_rows[pos].labels[0]->setToolTip(desc);
		m_rows[pos].check->setStyleSheet("QCheckBox { color: red; }");
		m_rows[pos].check->setToolTip(desc);
	}
}

QMap<QString, QStringList> SourcesWindow::loadPresets(QSettings *settings) const
{
	QMap<QString, QStringList> ret;

	const int size = settings->beginReadArray("SourcePresets");
	for (int i = 0; i < size; ++i) {
		settings->setArrayIndex(i);
		const QString name = settings->value("name").toString();
		const QStringList sources = settings->value("sources").toStringList();
		ret.insert(name, sources);
	}
	settings->endArray();

	return ret;
}

void SourcesWindow::savePresets(QSettings *settings) const
{
	settings->beginWriteArray("SourcePresets");
	int i = 0;
	for (auto it = m_presets.constBegin(); it != m_presets.constEnd(); ++it) {
		settings->setArrayIndex(i);
		settings->setValue("name", it.key());
		settings->setValue("sources", it.value());
		i++;
	}
	settings->endArray();
}

QList<Site*> SourcesWindow::selected() const
{
	QList<Site*> selected;
	for (const auto &row : qAsConst(m_rows)) {
		if (row.check->isChecked()) {
			selected.append(row.site);
		}
	}
	return selected;
}

void SourcesWindow::showPresets()
{
	// Reset combo box and re-add items
	ui->comboPresets->clear();
	ui->comboPresets->addItem(tr("- No preset selected -"));
	ui->comboPresets->addItems(m_presets.keys());

	// Set default preset font italic
	QFont font = ui->comboPresets->itemData(0, Qt::FontRole).value<QFont>();
	font.setItalic(true);
	ui->comboPresets->setItemData(0, font, Qt::FontRole);
}

void SourcesWindow::addPreset()
{
	bool ok;
	QString name = QInputDialog::getText(this, tr("Create a new preset"), tr("Name"), QLineEdit::Normal, QString(), &ok);
	if (!ok || name.isEmpty()) {
		return;
	}

	const QList<Site*> &selectedSites = selected();
	QStringList sel;
	sel.reserve(selectedSites.count());
	for (Site *site : selectedSites) {
		sel.append(site->url());
	}
	m_presets.insert(name, sel);

	showPresets();
	ui->comboPresets->setCurrentText(name);
}

void SourcesWindow::deletePreset()
{
	m_presets.remove(ui->comboPresets->currentText());
	ui->comboPresets->removeItem(ui->comboPresets->currentIndex());
}

void SourcesWindow::editPreset()
{
	bool ok;
	QString oldName = ui->comboPresets->currentText();
	QString newName = QInputDialog::getText(this, tr("Edit preset"), tr("Name"), QLineEdit::Normal, oldName, &ok);
	if (!ok || newName.isEmpty()) {
		return;
	}

	m_presets.insert(newName, m_presets[oldName]);
	m_presets.remove(oldName);

	showPresets();
	ui->comboPresets->setCurrentText(newName);
}

void SourcesWindow::savePreset()
{
	const QList<Site*> &selectedSites = selected();

	QStringList sel;
	sel.reserve(selectedSites.count());
	for (Site *site : selectedSites) {
		sel.append(site->url());
	}
	m_presets[ui->comboPresets->currentText()] = sel;

	ui->buttonPresetSave->setEnabled(false);
}


void SourcesWindow::selectPreset(const QString &name)
{
	bool isPreset = ui->comboPresets->currentIndex() > 0;

	if (isPreset) {
		const QStringList &preset = m_presets[name];
		for (const auto &row : qAsConst(m_rows)) {
			row.check->setChecked(preset.contains(row.site->url()));
		}
	}

	ui->buttonPresetSave->setEnabled(false);
	ui->buttonPresetEdit->setEnabled(isPreset);
	ui->buttonPresetDelete->setEnabled(isPreset);
}
