#include "addgroupwindow.h"
#include "ui_addgroupwindow.h"



/**
 * Constructor of the AddGroupWindow class, generating its window.
 * @param	parent		The parent window
 */
AddGroupWindow::AddGroupWindow(QString selected, QStringList sites, Profile *profile, QWidget *parent)
	: QDialog(parent), ui(new Ui::AddGroupWindow), m_sites(sites), m_settings(profile->getSettings())
{
	ui->setupUi(this);

	ui->comboSites->addItems(m_sites);
	ui->comboSites->setCurrentIndex(m_sites.indexOf(selected));

	m_lineTags = new TextEdit(profile, this);
		QStringList completion;
		completion.append(profile->getAutoComplete());
		completion.append(profile->getCustomAutoComplete());
		QCompleter *completer = new QCompleter(completion, this);
		completer->setCaseSensitivity(Qt::CaseInsensitive);
		m_lineTags->setCompleter(completer);
	ui->formLayout->setWidget(1, QFormLayout::FieldRole, m_lineTags);
}

/**
 * Relays the informations to the parent window.
 */
void AddGroupWindow::ok()
{
	QStringList values = QStringList() << m_lineTags->toPlainText()
									   << QString::number(ui->spinPage->value())
									   << QString::number(ui->spinPP->value())
									   << QString::number(ui->spinLimit->value())
									   << (ui->checkBlacklist->isChecked() ? "true" : "false")
									   << m_sites.at(ui->comboSites->currentIndex())
									   << m_settings->value("Save/filename").toString()
									   << m_settings->value("Save/path").toString()
									   << "";
	emit sendData(values);
	close();
}
