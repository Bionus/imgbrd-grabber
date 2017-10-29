#include "sitewindow.h"
#include "ui_sitewindow.h"
#include <QFile>
#include <QPushButton>
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"
#include "models/source-guesser.h"
#include "helpers.h"
#include "functions.h"


SiteWindow::SiteWindow(Profile *profile, QMap<QString ,Site*> *sites, QWidget *parent)
	: QDialog(parent), ui(new Ui::SiteWindow), m_profile(profile), m_sites(sites)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);

	ui->progressBar->hide();
	ui->comboBox->setDisabled(true);
	ui->checkBox->setChecked(true);

	m_sources = Source::getAllSources(nullptr);
	for (Source *source : *m_sources)
	{
		ui->comboBox->addItem(QIcon(source->getPath() + "/icon.png"), source->getName());
	}
}

SiteWindow::~SiteWindow()
{
	delete ui;
}

void SiteWindow::accept()
{
	m_url = ui->lineEdit->text();
	if (!m_url.startsWith("http://") && !m_url.startsWith("https://"))
	{ m_url.prepend("http://"); }
	if (m_url.endsWith("/"))
	{ m_url = m_url.left(m_url.size() - 1); }

	// Check URL validity
	if (!QRegularExpression(R"(^(https?:\/\/)?([\da-z.-]+)\.([a-z.]{2,6})([\/\w .-]*)*\/?$)").match(m_url).hasMatch())
	{
		error(this, tr("The url you entered is not valid."));
		return;
	}

	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

	if (ui->checkBox->isChecked())
	{
		ui->progressBar->setValue(0);
		ui->progressBar->setMaximum(m_sources->count());
		ui->progressBar->show();

		SourceGuesser sourceGuesser(m_url, *m_sources);
		connect(&sourceGuesser, &SourceGuesser::progress, ui->progressBar, &QProgressBar::setValue);
		connect(&sourceGuesser, &SourceGuesser::finished, this, &SiteWindow::finish);
		sourceGuesser.start();

		return;
	}

	Source *src = nullptr;
	for (Source *source : *m_sources)
	{
		if (source->getName() == ui->comboBox->currentText())
		{
			src = source;
			break;
		}
	}
	finish(src);
}

void SiteWindow::finish(Source *src)
{
	if (ui->checkBox->isChecked())
	{
		ui->progressBar->hide();

		if (src == nullptr)
		{
			error(this, tr("Unable to guess site's type. Are you sure about the url?"));
			ui->comboBox->setDisabled(false);
			ui->checkBox->setChecked(false);
			ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
			ui->progressBar->hide();
			return;
		}
	}

	// Remove unnecessary prefix
	bool ssl = false;
	if (m_url.startsWith("http://"))
	{ m_url = m_url.mid(7); }
	else if (m_url.startsWith("https://"))
	{
		m_url = m_url.mid(8);
		ssl = true;
	}
	if (m_url.endsWith('/'))
	{ m_url = m_url.left(m_url.length() - 1); }

	Site *site = new Site(m_url, src);
	src->getSites().append(site);
	m_sites->insert(site->url(), site);

	// If the user wrote "https://" in the URL, we enable SSL for this site
	if (ssl)
	{ site->setSetting("ssl", true, false); }

	// Save new sites
	QFile f(src->getPath() + "/sites.txt");
	f.open(QIODevice::ReadOnly);
		QString sites = f.readAll();
	f.close();
	sites.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n");
	QStringList stes = sites.split("\r\n", QString::SkipEmptyParts);
	stes.append(site->url());
	stes.removeDuplicates();
	stes.sort();
	f.open(QIODevice::WriteOnly);
		f.write(stes.join("\r\n").toLatin1());
	f.close();

	m_profile->addSite(site);

	emit accepted();
	close();
}
