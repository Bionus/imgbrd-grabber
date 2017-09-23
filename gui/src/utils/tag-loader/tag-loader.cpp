#include "tag-loader.h"
#include "ui_tag-loader.h"
#include <QMessageBox>
#include "models/profile.h"
#include "models/site.h"
#include "models/api.h"
#include "tags/tag-api.h"
#include "tags/tag.h"
#include "helpers.h"


TagLoader::TagLoader(Profile *profile, QMap<QString, Site *> sites, QWidget *parent)
	: QDialog(), ui(new Ui::TagLoader), m_profile(profile), m_sites(sites)
{
	ui->setupUi(this);

	QStringList keys;
	for (QString key : sites.keys())
	{
		Site *site = sites[key];
		if (!getCompatibleApis(site).isEmpty())
		{
			m_options.append(key);
			keys.append(QString("%1 (%L2 tags)").arg(key).arg(site->tagDatabase()->count()));
		}
	}

	ui->comboSource->addItems(keys);
	ui->widgetProgress->hide();

	resize(size().width(), 0);
}

TagLoader::~TagLoader()
{
	delete ui;
}

QList<Api*> TagLoader::getCompatibleApis(Site *site) const
{
	QList<Api*> apis;
	for (Api *a : site->getApis())
		if (a->contains("Urls/TagApi"))
			apis.append(a);

	return apis;
}

void TagLoader::cancel()
{
	emit rejected();
	close();
	deleteLater();
}

void TagLoader::start()
{
	// Get site and API
	Site *site = m_sites.value(ui->comboSource->currentText());
	QList<Api*> apis = getCompatibleApis(site);
	Api *api = apis.first();
	site->tagDatabase()->load();

	ui->buttonStart->setEnabled(false);

	// Show progress bar
	ui->progressBar->setValue(0);
	ui->progressBar->setMinimum(0);
	ui->progressBar->setMaximum(0);
	ui->labelProgress->setText("0");
	ui->widgetProgress->show();

	// Load all tags
	QList<Tag> allTags;
	QList<Tag> tags;
	int page = 1;
	while (!tags.isEmpty() || page == 1)
	{
		// Load tags for the current page
		QEventLoop loop;
		TagApi *tagApi = new TagApi(m_profile, site, api, page, 500, this);
		connect(tagApi, &TagApi::finishedLoading, &loop, &QEventLoop::quit);
		tagApi->load();
		loop.exec();

		tags = tagApi->tags();
		allTags.append(tags);
		tagApi->deleteLater();

		ui->progressBar->setValue(page);
		ui->labelProgress->setText(QString("%1 - %2").arg(page).arg(allTags.count()));
		page++;
	}

	// Update tag database
	site->tagDatabase()->setTags(allTags);
	site->tagDatabase()->save();

	// Hide progress bar
	ui->buttonStart->setEnabled(true);
	ui->widgetProgress->hide();
	resize(size().width(), 0);

	QMessageBox::information(this, tr("Finished"), tr("%n tag(s) loaded", "", allTags.count()));
}
