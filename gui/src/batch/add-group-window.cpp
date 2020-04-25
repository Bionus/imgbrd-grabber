#include "batch/add-group-window.h"
#include <QCompleter>
#include <QSettings>
#include <QStringList>
#include <ui_add-group-window.h>
#include "downloader/download-query-group.h"
#include "models/profile.h"
#include "models/site.h"
#include "ui/text-edit.h"


AddGroupWindow::AddGroupWindow(Site *selected, Profile *profile, QWidget *parent)
	: QDialog(parent), ui(new Ui::AddGroupWindow), m_sites(profile->getSites()), m_settings(profile->getSettings())
{
	ui->setupUi(this);

	QStringList keys = m_sites.keys();
	ui->comboSites->addItems(keys);
	ui->comboSites->setCurrentIndex(keys.indexOf(selected->url()));

	auto *completer = new QCompleter(profile->getAutoComplete(), this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);

	m_lineTags = new TextEdit(profile, this);
	m_lineTags->setCompleter(completer);
	ui->formLayout->setWidget(1, QFormLayout::FieldRole, m_lineTags);
	setTabOrder(ui->comboSites, m_lineTags);

	m_linePostFiltering = new TextEdit(profile, this);
	m_linePostFiltering->setCompleter(completer);
	ui->formLayout->setWidget(5, QFormLayout::FieldRole, m_linePostFiltering);
	setTabOrder(ui->spinLimit, m_linePostFiltering);
}

/**
 * Relays the information to the parent window.
 */
void AddGroupWindow::ok()
{
	const QStringList tags = m_lineTags->toPlainText().split(' ', QString::SkipEmptyParts);
	const QStringList postFiltering = m_linePostFiltering->toPlainText().split(' ', QString::SkipEmptyParts);
	Site *site = m_sites.value(ui->comboSites->currentText());
	emit sendData(DownloadQueryGroup(tags, ui->spinPage->value(), ui->spinPP->value(), ui->spinLimit->value(), postFiltering, ui->checkBlacklist->isChecked(), site, m_settings->value("Save/filename").toString(), m_settings->value("Save/path").toString()));
	close();
}
