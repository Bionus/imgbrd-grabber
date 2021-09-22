#include "sources/site-window.h"
#include <QFile>
#include <QPushButton>
#include <QRegularExpression>
#include <ui_site-window.h>
#include "functions.h"
#include "helpers.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"
#include "models/source-guesser.h"


SiteWindow::SiteWindow(Profile *profile, QWidget *parent)
	: QDialog(parent), ui(new Ui::SiteWindow), m_profile(profile)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);

	ui->progressBar->hide();
	ui->comboBox->setDisabled(true);
	ui->checkBox->setChecked(true);

	m_sources = profile->getSources().values();
	for (Source *source : qAsConst(m_sources)) {
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
	if (!m_url.startsWith("http://") && !m_url.startsWith("https://")) {
		m_url.prepend("http://");
	}
	if (m_url.endsWith("/")) {
		m_url = m_url.left(m_url.size() - 1);
	}

	// Check URL validity
	if (!QRegularExpression(R"(^(https?:\/\/)?([\da-z.-]+)\.([a-z.]{2,6})([\/\w .-?]*)*\/?$)").match(m_url).hasMatch()) {
		error(this, tr("The url you entered is not valid."));
		return;
	}

	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

	if (ui->checkBox->isChecked()) {
		ui->progressBar->setValue(0);
		ui->progressBar->setMaximum(m_sources.count());
		ui->progressBar->show();

		SourceGuesser sourceGuesser(m_url, m_sources);
		connect(&sourceGuesser, &SourceGuesser::progress, ui->progressBar, &QProgressBar::setValue);
		connect(&sourceGuesser, &SourceGuesser::finished, this, &SiteWindow::finish);
		sourceGuesser.start();

		return;
	}

	Source *src = nullptr;
	for (Source *source : qAsConst(m_sources)) {
		if (source->getName() == ui->comboBox->currentText()) {
			src = source;
			break;
		}
	}
	finish(src);
}

void SiteWindow::finish(Source *src)
{
	if (src == nullptr) {
		error(this, tr("Unable to guess site's type. Are you sure about the url?"));
		ui->comboBox->setDisabled(false);
		ui->checkBox->setChecked(false);
		ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
		ui->progressBar->hide();
		return;
	}

	if (ui->checkBox->isChecked()) {
		ui->progressBar->hide();
	}

	// Remove unnecessary prefix
	bool ssl = false;
	if (m_url.startsWith("http://")) {
		m_url = m_url.mid(7);
	} else if (m_url.startsWith("https://")) {
		m_url = m_url.mid(8);
		ssl = true;
	}
	if (m_url.endsWith('/')) {
		m_url = m_url.left(m_url.length() - 1);
	}

	Site *site = new Site(m_url, src);
	m_profile->addSite(site);

	// If the user wrote "https://" in the URL, we enable SSL for this site
	if (ssl) {
		site->setSetting("ssl", true, false);
	}

	emit accepted();
	close();
}
