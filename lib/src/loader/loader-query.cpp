#include "loader/loader-query.h"
#include <QEventLoop>
#include <QtMath>
#include "loader/loader-data.h"
#include "models/image.h"
#include "models/page.h"
#include "models/post-filter.h"
#include "models/site.h"
#include "models/source.h"


LoaderQuery::LoaderQuery(Site *site, const QMap<QString, QVariant> &options)
	: m_site(site), m_options(options), m_finished(false), m_offset(0)
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
	if (m_finished)
		return ret;

	// Options
	Profile *profile = m_site->getSource()->getProfile();
	QStringList tags = m_options["tags"].toStringList();
	int page = m_options["page"].toInt();
	int perPage = m_options["perPage"].toInt();
	int limit = m_options["limit"].toInt();
	QStringList postFiltering = m_options["postFiltering"].toStringList();
	bool getBlacklisted = m_options["getBlacklisted"].toBool();
	// QStringList blacklist = m_options["blacklist"].toStringList();
	QList<QStringList> blacklist;

	// Load results
	QEventLoop loop;
	Page request(profile, m_site, QList<Site*>() << m_site, tags, page + m_offset, perPage, postFiltering, true, Q_NULLPTR);
	QObject::connect(&request, &Page::finishedLoading, &loop, &QEventLoop::quit);
	QObject::connect(&request, &Page::failedLoading, &loop, &QEventLoop::quit);
	request.load(false);
	loop.exec();

	// Add results to the data object
	for (const QSharedPointer<Image> &img : request.images())
	{
		// Skip blacklisted images
		if (!getBlacklisted && !PostFilter::blacklisted(img->tokens(profile), blacklist).empty())
		{
			ret.ignored.append(img);
			continue;
		}

		ret.results.append(img);
	}

	// Paging
	int pageCount = qCeil(static_cast<float>(limit) / perPage);
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
