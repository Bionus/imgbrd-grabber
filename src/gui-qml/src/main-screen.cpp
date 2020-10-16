#include "main-screen.h"
#include <QEventLoop>
#include <QSettings>
#include <QSslSocket>
#include "logger.h"
#include "models/image.h"
#include "models/page.h"
#include "models/profile.h"

#define DEFAULT_SITE "danbooru.donmai.us"
#define IMAGES_PER_PAGE 20


MainScreen::MainScreen(Profile *profile, QObject *parent)
	: QObject(parent), m_profile(profile)
{
	connect(&Logger::getInstance(), &Logger::newLog, this, &MainScreen::newLog);

	LOG(QStringLiteral("New session started."), Logger::Info);
	LOG(QStringLiteral("Software version: %1.").arg(VERSION), Logger::Info);
	#ifdef NIGHTLY
		LOG(QStringLiteral("Nightly version: %1.").arg(QString(NIGHTLY_COMMIT)), Logger::Info);
	#endif
	LOG(QStringLiteral("Software CPU architecture: %1.").arg(VERSION_PLATFORM), Logger::Info);
	#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
		LOG(QStringLiteral("Computer CPU architecture: %1.").arg(QSysInfo::currentCpuArchitecture()), Logger::Info);
		LOG(QStringLiteral("Qt CPU architecture: %1.").arg(QSysInfo::buildCpuArchitecture()), Logger::Info);
		LOG(QStringLiteral("Computer platform: %1.").arg(QSysInfo::prettyProductName()), Logger::Info);
	#endif
	LOG(QStringLiteral("Loading preferences from `%1`").arg(m_profile->getSettings()->fileName()), Logger::Info);
	LOG(QStringLiteral("Temporary path: `%1`").arg(m_profile->tempPath()), Logger::Info);
	LOG(QStringLiteral("Sources found: %1").arg(m_profile->getSites().count()), Logger::Info);

	if (!QSslSocket::supportsSsl()) {
		LOG(QStringLiteral("Missing SSL libraries"), Logger::Error);
	} else {
		LOG(QStringLiteral("SSL libraries: %1").arg(QSslSocket::sslLibraryVersionString()), Logger::Info);
	}
}

void MainScreen::search(const QString &query, int pageNumber)
{
	m_query = query;
	emit queryChanged();

	Site *site = m_profile->getSites().value(DEFAULT_SITE);
	Page *page = new Page(m_profile, site, m_profile->getSites().values(), query.split(' '), pageNumber, IMAGES_PER_PAGE, {}, false, this);

	QEventLoop loop;
	QObject::connect(page, &Page::finishedLoading, &loop, &QEventLoop::quit);
	QObject::connect(page, &Page::failedLoading, &loop, &QEventLoop::quit);
	page->load(false);
	loop.exec();

	QList<QSharedPointer<Image>> results = page->images();

	m_thumbnails.clear();
	m_thumbnails.reserve(results.count());
	for (const QSharedPointer<Image> &img : results) {
		m_thumbnails.append(new ImagePreview(img->url(Image::Size::Thumbnail).toString(), this));
	}

	emit thumbnailsChanged();
}

void MainScreen::newLog(const QString &message)
{
	m_log += message + "\n";
	emit logChanged();
}
