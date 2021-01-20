#include "utils/tag-loader/tag-loader.h"
#include <QMessageBox>
#include <ui_tag-loader.h>
#include "helpers.h"
#include "models/profile.h"
#include "models/site.h"
#include "tags/tag-database.h"
#include "tools/tag-list-loader.h"

#define MIN_TAG_COUNT 20


TagLoader::TagLoader(Profile *profile, QWidget *parent)
	: QDialog(parent), ui(new Ui::TagLoader), m_profile(profile), m_sites(profile->getSites())
{
	ui->setupUi(this);

	resetOptions();
	ui->widgetProgress->hide();

	resize(size().width(), 0);
}

TagLoader::~TagLoader()
{
	delete ui;
}

void TagLoader::resetOptions()
{
	m_options.clear();

	QStringList keys;
	for (auto it = m_sites.constBegin(); it != m_sites.constEnd(); ++it) {
		Site *site = it.value();
		if (TagListLoader::canLoadTags(site)) {
			m_options.append(it.key());
			keys.append(QString("%1 (%L2 tags)").arg(it.key()).arg(site->tagDatabase()->count()));
		}
	}

	int index = ui->comboSource->currentIndex();
	ui->comboSource->clear();
	ui->comboSource->addItems(keys);
	if (index >= 0) {
		ui->comboSource->setCurrentIndex(index);
	}
}

void TagLoader::cancel()
{
	if (m_loader != nullptr) {
		m_loader->cancel();
	}

	emit rejected();
	close();
	deleteLater();
}

void TagLoader::start()
{
	Site *site = m_sites.value(m_options[ui->comboSource->currentIndex()]);

	// Show progress bar
	ui->buttonStart->setEnabled(false);
	ui->progressBar->setValue(0);
	ui->progressBar->setMinimum(0);
	ui->progressBar->setMaximum(0);
	ui->labelProgress->setText("");
	ui->widgetProgress->show();

	// Start loader
	m_loader = new TagListLoader(m_profile, site, MIN_TAG_COUNT, this);
	connect(m_loader, TagListLoader::progress, ui->labelProgress, &QLabel::setText);
	connect(m_loader, TagListLoader::finished, this, &TagLoader::finishedLoading);
	m_loader->start();
}

void TagLoader::finishedLoading()
{
	// Hide progress bar
	ui->buttonStart->setEnabled(true);
	ui->widgetProgress->hide();
	resize(size().width(), 0);

	// Handle errors
	if (!m_loader->error().isEmpty()) {
		error(this, m_loader->error());
	} else {
		QMessageBox::information(this, tr("Finished"), tr("%n tag(s) loaded", "", m_loader->results().count()));
	}

	// Clean-up
	m_loader->deleteLater();
	m_loader = nullptr;

	resetOptions();
}
