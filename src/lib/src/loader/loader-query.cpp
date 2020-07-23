#include "loader/loader-query.h"
#include <QEventLoop>
#include <QtMath>
#include <utility>
#include "loader/loader-data.h"
#include "models/filtering/blacklist.h"
#include "models/image.h"
#include "models/page.h"
#include "models/site.h"
#include "models/source.h"


LoaderQuery::LoaderQuery(Site *site, QMap<QString, QVariant> options)
	: m_site(site), m_options(std::move(options)), m_finished(false), m_offset(0)
{}

bool LoaderQuery::start()
{
	QEventLoop loop;
	QObject::connect(m_site, &Site::loggedIn, &loop, &QEventLoop::quit);
	m_site->login();
	loop.exec();

	return true;
}

LoaderData LoaderQuery::next()
{
	LoaderData ret;

	// Early return if we try to load the next results of a finished query
	if (m_finished) {
		return ret;
	}

	// Options
	Profile *profile = m_site->getSource()->getProfile();
	const QStringList tags = m_options["tags"].toStringList();
	const int page = m_options["page"].toInt();
	const int perPage = m_options["perPage"].toInt();
	const int limit = m_options["limit"].toInt();
	const QStringList postFiltering = m_options["postFiltering"].toStringList();
	const bool getBlacklisted = m_options["getBlacklisted"].toBool();
	// const QStringList blacklist = m_options["blacklist"].toStringList();
	const Blacklist blacklist {};

	// Load results
	QEventLoop loop;
	Page request(profile, m_site, { m_site }, tags, page + m_offset, perPage, postFiltering, true, nullptr);
	QObject::connect(&request, &Page::finishedLoading, &loop, &QEventLoop::quit);
	QObject::connect(&request, &Page::failedLoading, &loop, &QEventLoop::quit);
	request.load(false);
	loop.exec();

	// Add results to the data object
	const QList<QSharedPointer<Image>> &images = request.images();
	for (const QSharedPointer<Image> &img : images) {
		// Skip blacklisted images
		if (!getBlacklisted && !blacklist.match(img->tokens(profile)).empty()) {
			ret.ignored.append(img);
			continue;
		}

		ret.results.append(img);
	}

	// Paging
	const int pageCount = qCeil(static_cast<qreal>(limit) / perPage);
	m_offset++;
	m_finished = m_offset == pageCount;

	ret.position = m_offset;
	ret.max = pageCount;

	return ret;
}

bool LoaderQuery::hasNext() const
{
	return !m_finished;
}
