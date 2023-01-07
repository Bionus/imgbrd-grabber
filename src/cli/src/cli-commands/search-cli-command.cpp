#include "search-cli-command.h"
#include <QEventLoop>
#include <utility>
#include "models/page.h"


SearchCliCommand::SearchCliCommand(Profile *profile, QStringList tags, QStringList postFiltering, QList<Site*> sites, int page, int perPage, QObject *parent)
	: CliCommand(parent), m_profile(profile), m_tags(std::move(tags)), m_postFiltering(std::move(postFiltering)), m_sites(std::move(sites)), m_page(page), m_perPage(perPage)
{}

QList<Page*> SearchCliCommand::getTagsForAllPages()
{
	QList<Page*> pages;

	for (auto *site : m_sites) {
		auto *page = new Page(m_profile, site, m_sites, m_tags, m_page, m_perPage, m_postFiltering, true, this);

		QEventLoop loop;
		connect(page, &Page::finishedLoadingTags, &loop, &QEventLoop::quit, Qt::QueuedConnection);
		page->loadTags();
		loop.exec();

		pages.append(page);
	}

	return pages;
}
