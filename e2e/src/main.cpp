#include <QCoreApplication>
#include <QDebug>
#include <QNetworkAccessManager>
#include "functions.h"
#include "models/api/api.h"
#include "models/page.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"


int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	auto manager = new QNetworkAccessManager();
	QStringList sourcesToTest = QStringList() << "Danbooru (2.0)";
	QStringList sitesToTest = QStringList() << "danbooru.donmai.us";

	Profile *profile = new Profile(savePath());
	for (Source *source : profile->getSources().values())
	{
		if (!sourcesToTest.isEmpty() && !sourcesToTest.contains(source->getName()))
			continue;

		qDebug() << "#########" << "Source" << source->getName();

		for (Site *site : source->getSites())
		{
			if (!sitesToTest.isEmpty() && !sitesToTest.contains(site->url()))
				continue;

			qDebug() << "######" << "Site" << site->url();

			QString search = "rating:safe";
			int pagei = 1;
			int limit = 5;

			Page *page = new Page(profile, site, profile->getSites().values(), QStringList() << search, pagei, limit);

			for (Api *api : site->getApis())
			{
				qDebug() << "###" << "API" << api->getName();

				QString url = api->pageUrl(search, pagei, limit, 0, 0, 0, site);
				QEventLoop loop;
				auto reply = manager->get(QNetworkRequest(QUrl(url)));
				QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
				loop.exec();

				QString src = reply->readAll();
				qDebug() << "url" << url;
				qDebug() << "src" << src.left(200);

				auto ret = api->parsePage(page, src, 0);
				qDebug() << "error" << ret.error;
				qDebug() << "imagesCount" << ret.imagesCount;
				qDebug() << "images" << ret.images.count();
				for (const QSharedPointer<Image> &img : ret.images)
				{ qDebug() << "-" << img->md5(); }
				qDebug() << "tags" << ret.tags.count();
				for (const Tag &tag : ret.tags)
				{ qDebug() << "-" << tag.text() << "/" << tag.type().name() << "/" << tag.count(); }
			}

		}
	}

	manager->deleteLater();

	return 0;
}
