#include "get-images-cli-command.h"
#include <QList>
#include <utility>
#include "downloader/printers/printer.h"
#include "logger.h"
#include "models/page.h"


GetImagesCliCommand::GetImagesCliCommand(Profile *profile, Printer *printer, QStringList tags, QStringList postFiltering, QList<Site*> sites, int page, int perPage, QString filename, QString folder, int max, bool login, bool noDuplicates, bool getBlacklisted, bool loadMoreDetails, QObject *parent)
	: SearchImagesCliCommand(profile, std::move(tags), std::move(postFiltering), std::move(sites), page, perPage, filename, folder, max, login, noDuplicates, getBlacklisted, parent), m_printer(printer), m_loadMoreDetails(loadMoreDetails)
{}

bool GetImagesCliCommand::validate()
{
	if (!SearchImagesCliCommand::validate()) {
		return false;
	}

	return true;
}

void GetImagesCliCommand::run()
{
	const QList<QSharedPointer<Image>> images = getAllImages();

	if (m_loadMoreDetails) {
		loadMoreDetails(images);
	}

	m_printer->print(images);

	emit finished(0);
}
