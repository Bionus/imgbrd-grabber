#include "get-details-cli-command.h"
#include <QList>
#include "cli-command.h"
#include "logger.h"
#include "models/image.h"
#include "downloader/printers/printer.h"


GetDetailsCliCommand::GetDetailsCliCommand(Profile *profile, Printer *printer, const QList<Site*> &sites, const QString &pageUrl, QObject *parent)
	: CliCommand(parent), m_profile(profile), m_printer(printer), m_sites(sites), m_pageUrl(pageUrl)
{}

bool GetDetailsCliCommand::validate()
{
	if (m_sites.count() != 1) {
		log("You must provide exactly one source to load the details from", Logger::Error);
		return false;
	}

	if (m_pageUrl.isEmpty()) {
		log("You must pass a page URL to load the details", Logger::Error);
		return false;
	}

	return true;
}

void GetDetailsCliCommand::run()
{
	QMap<QString, QString> details = {{ "page_url", m_pageUrl }};

	m_image = new Image(m_sites[0], details, m_profile);
	m_image->setPromoteDetailParsWarn(true);

	connect(m_image, &Image::finishedLoadingTags, this, &GetDetailsCliCommand::finishedLoading);
	m_image->loadDetails();
}

void GetDetailsCliCommand::finishedLoading()
{
	m_printer->print(*m_image);
	m_image->deleteLater();

	emit finished(0);
}
