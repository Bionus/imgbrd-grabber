#include "get-page-tags-cli-command.h"
#include <QList>
#include <utility>
#include "downloader/printers/printer.h"
#include "logger.h"
#include "models/page.h"


GetPageTagsCliCommand::GetPageTagsCliCommand(Profile *profile, Printer *printer, QStringList tags, QStringList postFiltering, QList<Site*> sources, int page, int perPage, int tagsMin, QObject *parent)
	: SearchCliCommand(profile, std::move(tags), std::move(postFiltering), std::move(sources), page, perPage, parent), m_printer(printer), m_tagsMin(tagsMin)
{}

bool GetPageTagsCliCommand::validate()
{
	if (m_sites.isEmpty()) {
		log("You must provide at least one source to load the page tags from", Logger::Error);
		return false;
	}

	return true;
}

void GetPageTagsCliCommand::run()
{
	const QList<Page*> pages = getTagsForAllPages();

	QList<Tag> tags;
	for (Page *p : pages) {
		const QList<Tag> &pageTags = p->tags();
		for (const Tag &tag : pageTags) {
			bool found = false;
			for (auto &t : tags) {
				if (t.text() == tag.text()) {
					t.setCount(t.count() + tag.count());
					found = true;
				}
			}
			if (!found) {
				tags.append(tag);
			}
		}
	}

	qDeleteAll(pages);

	QMutableListIterator<Tag> i(tags);
	while (i.hasNext()) {
		if (i.next().count() < m_tagsMin) {
			i.remove();
		}
	}

	m_printer->print(tags);

	emit finished(0);
}
