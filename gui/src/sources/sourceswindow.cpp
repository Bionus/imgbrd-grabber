#include "sourceswindow.h"
#include "ui_sourceswindow.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QInputDialog>
#include "ui/QBouton.h"
#include "sitewindow.h"
#include "sourcessettingswindow.h"
#include "models/source.h"
#include "functions.h"


/**
 * Constructor of the sourcesWindow, generating checkboxes and delete buttons
 * @param	selected	Bool list of currently selected websites, in the alphabetical order
 * @param	sites		QStringList of sites names
 * @param	parent		The parent window
 */
sourcesWindow::sourcesWindow(Profile *profile, QList<bool> selected, QMap<QString, Site*> *sites, QWidget *parent)
	: QDialog(parent), ui(new Ui::sourcesWindow), m_profile(profile), m_selected(selected), m_sites(sites)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	restoreGeometry(m_profile->getSettings()->value("Sources/geometry").toByteArray());

	bool checkall = true;
	for (int i = 0; i < selected.count(); i++)
	{
		if (!selected.at(i))
		{
			checkall = false;
			break;
		}
	}
	if (checkall)
	{ ui->checkBox->setChecked(true); }

	addCheckboxes();

	ui->gridLayout->setColumnStretch(0, 1);
	connect(ui->checkBox, SIGNAL(clicked()), this, SLOT(checkClicked()));
	checkUpdate();

	QStringList keys = m_sites->keys();
	for (int i = 0; i < m_sites->size(); i++)
	{
		Source *source = m_sites->value(keys[i])->getSource();
		m_sources[source->getName()] = source;
	}

	// Presets
	m_presets = loadPresets(m_profile->getSettings());
	ui->comboPresets->addItems(m_presets.keys());

	// Check for updates in the model files
	checkForUpdates();

	ui->buttonOk->setFocus();
}
sourcesWindow::~sourcesWindow()
{
	delete ui;
}

/**
 * When closed, the window emit a signal which will trigger a slot in the main window.
 * @param	event	The event triggered wy window's closing
 * @todo	Why use a signal, since we can simply use a pointer to the parent window, or a signal giving not a pointer to this window, but directly m_selected ?
 */
void sourcesWindow::closeEvent(QCloseEvent *event)
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
void sourcesWindow::checkUpdate()
{
	bool oneChecked = false;
	bool oneUnchecked = false;
	for (QCheckBox *check : m_checks)
	{
		if (check->isChecked())
		{ oneChecked = true; }
		else
		{ oneUnchecked = true; }
	}
	if (oneChecked && !oneUnchecked)
	{ ui->checkBox->setCheckState(Qt::Checked); }
	else if (!oneChecked && oneUnchecked)
	{ ui->checkBox->setCheckState(Qt::Unchecked); }
	else
	{ ui->checkBox->setCheckState(Qt::PartiallyChecked); }

	// Update preset save button
	if (ui->comboPresets->currentIndex() > 0)
	{ ui->buttonPresetSave->setEnabled(true); }
}

/**
 * Alternate between the checked and unchecked state of the tri-state checkbox "Check all".
 */
void sourcesWindow::checkClicked()
{
	if (ui->checkBox->checkState() == Qt::Unchecked)
	{ ui->checkBox->setCheckState(Qt::Unchecked); }
	else
	{ ui->checkBox->setCheckState(Qt::Checked); }
	checkAll(ui->checkBox->checkState());
}

/**
 * Saves current selection then close the window.
 */
void sourcesWindow::valid()
{
	for (int i = 0; i < m_selected.count(); i++)
		m_selected[i] = m_checks.at(i)->isChecked();

	emit valid(m_selected);
	this->close();
}

void sourcesWindow::settingsSite(QString site)
{
	SourcesSettingsWindow *ssw = new SourcesSettingsWindow(m_profile, m_sites->value(site), this);
	connect(ssw, SIGNAL(siteDeleted(QString)), this, SLOT(deleteSite(QString)));
	ssw->show();
}

/**
 * Delete a site from the sources list.
 * @param	site	The url of the site to delete.
 */
void sourcesWindow::deleteSite(QString site)
{
	int i = m_sites->keys().indexOf(site);

	m_checks.at(i)->hide();
	ui->gridLayout->removeWidget(m_checks.at(i));
	m_checks.removeAt(i);
	m_buttons.at(i)->hide();
	ui->gridLayout->removeWidget(m_buttons.at(i));
	m_buttons.removeAt(i);
	if (!m_labels.isEmpty())
	{
		m_labels.at(i)->hide();
		ui->gridLayout->removeWidget(m_labels.at(i));
		m_labels.removeAt(i);
	}

	m_sites->remove(site);
	m_selected.removeAt(i);
}

/**
 * Open the window to add a site.
 */
void sourcesWindow::addSite()
{
	auto *sw = new SiteWindow(m_profile, m_sites, this);
	connect(sw, &SiteWindow::accepted, this, &sourcesWindow::updateCheckboxes);
	sw->show();
}

void sourcesWindow::updateCheckboxes()
{
	QStringList k = m_sites->keys();
	for (int i = 0; i < k.count(); i++)
	{
		if (m_checks.at(i)->text() != k.at(i))
		{
			m_selected.insert(i, true);
			break;
		}
	}

	removeCheckboxes();
	addCheckboxes();
}

void sourcesWindow::removeCheckboxes()
{
	for (int i = 0; i < m_checks.count(); i++)
	{
		ui->gridLayout->removeWidget(m_checks.at(i));
		m_checks.at(i)->deleteLater();

		ui->gridLayout->removeWidget(m_buttons.at(i));
		m_buttons.at(i)->deleteLater();

		if (!m_labels.isEmpty())
		{
			m_labels.at(i)->deleteLater();
			ui->gridLayout->removeWidget(m_labels.at(i));
		}
	}

	m_checks.clear();
	m_buttons.clear();
	m_labels.clear();
}

/**
 * Add a site to the list.
 */
void sourcesWindow::addCheckboxes()
{
	QString t = m_profile->getSettings()->value("Sources/Types", "icon").toString();

	QStringList k = m_sites->keys();
	for (int i = 0; i < k.count(); i++)
	{
		auto *check = new QCheckBox(this);
			check->setChecked(m_selected.size() > i && m_selected[i]);
			check->setText(k.at(i));
			connect(check, SIGNAL(stateChanged(int)), this, SLOT(checkUpdate()));
			m_checks << check;
			ui->gridLayout->addWidget(check, i, 0);

		int n = 1;
		if (t != "hide")
		{
			if (t == "icon" || t == "both")
			{
				QLabel *image = new QLabel(this);
				image->setPixmap(QPixmap(m_sites->value(k.at(i))->getSource()->getPath() + "/icon.png").scaled(QSize(16, 16)));
				ui->gridLayout->addWidget(image, i, n);
				m_labels << image;
				n++;
			}
			if (t == "text" || t == "both")
			{
				QLabel *type = new QLabel(m_sites->value(k.at(i))->value("Name"), this);
				ui->gridLayout->addWidget(type, i, n);
				m_labels << type;
				n++;
			}
		}

		QBouton *del = new QBouton(k.at(i));
			del->setParent(this);
			del->setText(tr("Options"));
			connect(del, SIGNAL(appui(QString)), this, SLOT(settingsSite(QString)));
			m_buttons << del;
			ui->gridLayout->addWidget(del, i, n);
	}

	/*int n =  0+(t == "icon" || t == "both")+(t == "text" || t == "both");
	for (int i = 0; i < m_checks.count(); i++)
	{
		ui->gridLayout->addWidget(m_checks.at(i), i, 0);
		m_checks.at(i)->show();
		if (!m_labels.isEmpty())
		{
			for (int r = 0; r < n; r++)
			{
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
void sourcesWindow::checkAll(int check)
{
	for (int i = 0; i < m_checks.count(); i++)
		m_checks.at(i)->setChecked(check == 2);
}

/**
 * Accessor for the "selected" variable.
 *
 * @return A bool list corresponding to selected websites.
 */
QList<bool> sourcesWindow::getSelected()
{ return m_selected; }

void sourcesWindow::checkForUpdates()
{
	for (Source *source : m_sources.values())
	{
		SourceUpdater *updater = source->getUpdater();
		connect(updater, &SourceUpdater::finished, this, &sourcesWindow::checkForUpdatesReceived);
		updater->checkForUpdates();
	}
}
void sourcesWindow::checkForUpdatesReceived(QString sourceName, bool isNew)
{
	if (!isNew)
		return;

	Source *source = m_sources[sourceName];
	for (Site *site : source->getSites())
	{
		int pos = m_sites->values().indexOf(site);

		m_labels[pos]->setPixmap(QPixmap(":/images/icons/update.png"));
		m_labels[pos]->setToolTip(tr("An update for this source is available."));
	}
}

QMap<QString, QStringList> sourcesWindow::loadPresets(QSettings *settings) const
{
	QMap<QString, QStringList> ret;

	int size = settings->beginReadArray("SourcePresets");
	for (int i = 0; i < size; ++i)
	{
		settings->setArrayIndex(i);
		QString name = settings->value("name").toString();
		QStringList sources = settings->value("sources").toStringList();
		ret.insert(name, sources);
	}
	settings->endArray();

	return ret;
}

void sourcesWindow::savePresets(QSettings *settings) const
{
	QStringList names = m_presets.keys();
	settings->beginWriteArray("SourcePresets");
	for (int i = 0; i < names.count(); ++i)
	{
		settings->setArrayIndex(i);
		QString name = names[i];
		settings->setValue("name", name);
		settings->setValue("sources", m_presets[name]);
	}
	settings->endArray();
}

QList<Site*> sourcesWindow::selected() const
{
	QList<Site*> selected;

	const QStringList &keys = m_sites->keys();
	for (int i = 0; i < keys.count(); ++i)
		if (m_checks[i]->isChecked())
			selected.append(m_sites->value(keys[i]));

	return selected;
}

void sourcesWindow::addPreset()
{
	QString name = QInputDialog::getText(this, tr("Create a new preset"), tr("Name"));

	QStringList sel;
	for (Site *site : selected())
		sel.append(site->url());
	m_presets.insert(name, sel);

	ui->comboPresets->clear();
	ui->comboPresets->addItem("");
	ui->comboPresets->addItems(m_presets.keys());
	ui->comboPresets->setCurrentText(name);
}

void sourcesWindow::deletePreset()
{
	m_presets.remove(ui->comboPresets->currentText());
	ui->comboPresets->removeItem(ui->comboPresets->currentIndex());
}

void sourcesWindow::editPreset()
{
	QString oldName = ui->comboPresets->currentText();
	QString newName = QInputDialog::getText(this, tr("Edit preset"), tr("Name"), QLineEdit::Normal, oldName);

	m_presets.insert(newName, m_presets[oldName]);
	m_presets.remove(oldName);

	ui->comboPresets->clear();
	ui->comboPresets->addItem("");
	ui->comboPresets->addItems(m_presets.keys());
	ui->comboPresets->setCurrentText(newName);
}

void sourcesWindow::savePreset()
{
	QStringList sel;
	for (Site *site : selected())
		sel.append(site->url());
	m_presets[ui->comboPresets->currentText()] = sel;

	ui->buttonPresetSave->setEnabled(false);
}


void sourcesWindow::selectPreset(QString name)
{
	bool isPreset = ui->comboPresets->currentIndex() > 0;

	if (isPreset)
	{
		const QStringList &preset = m_presets[name];
		const QStringList &keys = m_sites->keys();
		for (int i = 0; i < keys.count(); ++i)
		{
			Site *site = m_sites->value(keys[i]);
			m_checks[i]->setChecked(preset.contains(site->url()));
		}
	}

	ui->buttonPresetSave->setEnabled(false);
	ui->buttonPresetEdit->setEnabled(isPreset);
	ui->buttonPresetDelete->setEnabled(isPreset);
}
