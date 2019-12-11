#include "utils/tag-loader/tag-loader.h"
#include <QMessageBox>
#include <ui_tag-loader.h>
#include "helpers.h"
#include "models/api/api.h"
#include "models/profile.h"
#include "models/site.h"
#include "tags/tag.h"
#include "tags/tag-api.h"
#include "tags/tag-database.h"
#include "tags/tag-type-api.h"


TagLoader::TagLoader(Profile *profile, QWidget *parent)
	: QDialog(parent), ui(new Ui::TagLoader), m_profile(profile), m_sites(profile->getSites())
{
	ui->setupUi(this);

	QStringList keys;
	for (auto it = m_sites.constBegin(); it != m_sites.constEnd(); ++it) {
		Site *site = it.value();
		bool hasApiForTags = !getApisToLoadTags(site).isEmpty();
		bool hasApiForTagTypes = !getApisToLoadTagTypes(site).isEmpty();
		bool needTagTypes = site->tagDatabase()->tagTypes().isEmpty();
		if (hasApiForTags && (!needTagTypes || hasApiForTagTypes)) {
			m_options.append(it.key());
			keys.append(QString("%1 (%L2 tags)").arg(it.key()).arg(site->tagDatabase()->count()));
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

QList<Api*> TagLoader::getApisToLoadTagTypes(Site *site) const
{
	QList<Api*> apis;
	for (Api *a : site->getApis()) {
		if (a->canLoadTagTypes()) {
			apis.append(a);
		}
	}

	return apis;
}

QList<Api*> TagLoader::getApisToLoadTags(Site *site) const
{
	QList<Api*> apis;
	for (Api *a : site->getApis()) {
		if (a->canLoadTags()) {
			apis.append(a);
		}
	}

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
	Site *site = m_sites.value(m_options[ui->comboSource->currentIndex()]);

	// Load tag types first if necessary
	if (site->tagDatabase()->tagTypes().isEmpty()) 	{
		ui->labelProgress->setText(tr("Loading tag types..."));

		// Get tag type loading API
		QList<Api*> apisTypes = getApisToLoadTagTypes(site);
		Api *apiTypes = apisTypes.first();

		// Load tag types
		QEventLoop loop;
		auto *tagTypeApi = new TagTypeApi(m_profile, site, apiTypes, this);
		connect(tagTypeApi, &TagTypeApi::finishedLoading, &loop, &QEventLoop::quit);
		tagTypeApi->load();
		loop.exec();

		auto tagTypes = tagTypeApi->tagTypes();
		if (tagTypes.isEmpty()) {
			error(this, tr("Error loading tag types."));
			return;
		}
		site->tagDatabase()->setTagTypes(tagTypes);
	}

	// Get tag loading API
	QList<Api*> apis = getApisToLoadTags(site);
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
	while (!tags.isEmpty() || page == 1) {
		// Load tags for the current page
		QEventLoop loop;
		auto *tagApi = new TagApi(m_profile, site, api, page, 500, this);
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
