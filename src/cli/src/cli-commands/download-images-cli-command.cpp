#include "download-images-cli-command.h"
#include <QEventLoop>
#include <QList>
#include <utility>
#include "downloader/image-downloader.h"
#include "printers/printer.h"
#include "logger.h"


DownloadImagesCliCommand::DownloadImagesCliCommand(Profile *profile, Printer *printer, QStringList tags, QStringList postFiltering, QList<Site*> sites, int page, int perPage, QString filename, QString folder, int max, bool login, bool noDuplicates, bool getBlacklisted, Blacklist blacklistedTags, QObject *parent)
	: SearchImagesCliCommand(profile, std::move(tags), std::move(postFiltering), std::move(sites), page, perPage, std::move(filename), std::move(folder), max, login, noDuplicates, getBlacklisted, parent), m_printer(printer), m_blacklistedTags(std::move(blacklistedTags))
{}

bool DownloadImagesCliCommand::validate()
{
	if (!SearchImagesCliCommand::validate()) {
		return false;
	}

	if (m_filename.isEmpty()) {
		log("You need a filename for downloading images", Logger::Error);
		return false;
	}

	return true;
}

void DownloadImagesCliCommand::run()
{
	const QList<QSharedPointer<Image>> images = getAllImages();

	for (const auto &image : images) {
		ImageDownloader dwl(m_profile, image, m_filename, m_folder, 0, true, false, this);
		if (!m_getBlacklisted) {
			dwl.setBlacklist(&m_blacklistedTags);
		}

		QEventLoop loop;
		QObject::connect(&dwl, &ImageDownloader::saved, &loop, &QEventLoop::quit, Qt::QueuedConnection);
		dwl.save();
		loop.exec();
	}

	m_printer->print(QStringLiteral("Downloaded images successfully."));

	emit finished(0);
}
