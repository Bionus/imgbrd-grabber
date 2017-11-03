#include "addgroupwindow.h"
#include "ui_addgroupwindow.h"
#include "ui/textedit.h"
#include "models/profile.h"
#include "models/site.h"
#include "downloader/download-query-group.h"


/**
 * Constructor of the AddGroupWindow class, generating its window.
 * @param	parent		The parent window
 */
AddGroupWindow::AddGroupWindow(QString selected, QMap<QString, Site*> sites, Profile *profile, QWidget *parent)
	: QDialog(parent), ui(new Ui::AddGroupWindow), m_sites(sites), m_settings(profile->getSettings())
{
	ui->setupUi(this);

	QStringList keys = m_sites.keys();
	ui->comboSites->addItems(keys);
	ui->comboSites->setCurrentIndex(keys.indexOf(selected));

	QStringList completion;
	completion.append(profile->getAutoComplete());
	completion.append(profile->getCustomAutoComplete());
	auto *completer = new QCompleter(completion, this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);

	m_lineTags = new TextEdit(profile, this);
	m_lineTags->setCompleter(completer);
	ui->formLayout->setWidget(1, QFormLayout::FieldRole, m_lineTags);

	m_linePostFiltering = new TextEdit(profile, this);
	m_linePostFiltering->setCompleter(completer);
	ui->formLayout->setWidget(5, QFormLayout::FieldRole, m_linePostFiltering);
}

/**
 * Relays the information to the parent window.
 */
void AddGroupWindow::ok()
{
	QStringList postFiltering = m_linePostFiltering->toPlainText().split(' ', QString::SkipEmptyParts);
	Site *site = m_sites.value(ui->comboSites->currentText());
	emit sendData(DownloadQueryGroup(m_lineTags->toPlainText(), ui->spinPage->value(), ui->spinPP->value(), ui->spinLimit->value(), postFiltering, ui->checkBlacklist->isChecked(), site, m_settings->value("Save/filename").toString(), m_settings->value("Save/path").toString()));
	close();
}
