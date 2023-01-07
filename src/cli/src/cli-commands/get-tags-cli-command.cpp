#include "get-tags-cli-command.h"
#include <QEventLoop>
#include <QList>
#include <QtMath>
#include "cli-command.h"
#include "printers/printer.h"
#include "logger.h"
#include "models/site.h"
#include "tags/tag-api.h"


GetTagsCliCommand::GetTagsCliCommand(Profile *profile, Printer *printer, const QList<Site*> &sites, int page, int perPage, int max, int tagsMin, QObject *parent)
	: CliCommand(parent), m_profile(profile), m_printer(printer), m_sites(sites), m_page(page), m_perPage(perPage), m_max(max), m_tagsMin(tagsMin)
{}

bool GetTagsCliCommand::validate()
{
	if (m_sites.isEmpty()) {
		log("You must provide at least one source to load the tags from", Logger::Error);
		return false;
	}

	return true;
}

void GetTagsCliCommand::run()
{
	QList<Tag> results;

	for (Site *site : qAsConst(m_sites)) {
		Api *api = site->tagsApi();
		if (api == nullptr) {
			log(QStringLiteral("No valid API for loading tags for source: %1").arg(site->url()), Logger::Error);
			return;
		}

		int pages = qCeil(static_cast<qreal>(m_max) / m_perPage);
		if (pages <= 0 || m_perPage <= 0 || m_max <= 0) {
			pages = 1;
		}

		for (int p = 0; p < pages; ++p) {
			auto *tagApi = new TagApi(m_profile, site, api, m_page + p, m_perPage, "count", this);

			QEventLoop loop;
			QObject::connect(tagApi, &TagApi::finishedLoading, &loop, &QEventLoop::quit, Qt::QueuedConnection);
			tagApi->load();
			loop.exec();

			const QList<Tag> tags = tagApi->tags();
			log(QStringLiteral("Received pure tags (%1)").arg(tags.count()));
			tagApi->deleteLater();

			results.append(tags);
		}
	}

	QMutableListIterator<Tag> i(results);
	while (i.hasNext()) {
		if (i.next().count() < m_tagsMin) {
			i.remove();
		}
	}

	m_printer->print(results, nullptr);

	emit finished(0);
}
