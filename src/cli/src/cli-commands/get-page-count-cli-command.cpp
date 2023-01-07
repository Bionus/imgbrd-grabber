#include "get-page-count-cli-command.h"
#include <QList>
#include <utility>
#include "printers/printer.h"
#include "logger.h"
#include "models/page.h"


GetPageCountCliCommand::GetPageCountCliCommand(Profile *profile, Printer *printer, QStringList tags, QStringList postFiltering, QList<Site*> sources, int page, int perPage, QObject *parent)
	: SearchCliCommand(profile, std::move(tags), std::move(postFiltering), std::move(sources), page, perPage, parent), m_printer(printer)
{}

bool GetPageCountCliCommand::validate()
{
	if (m_sites.isEmpty()) {
		log("You must provide at least one source to load the page count of", Logger::Error);
		return false;
	}

	return true;
}

void GetPageCountCliCommand::run()
{
	const QList<Page*> pages = getTagsForAllPages();

	int total = 0;
	for (Page *p : pages) {
		total += p->imagesCount();
	}

	qDeleteAll(pages);

	m_printer->print(total);

	emit finished(0);
}
