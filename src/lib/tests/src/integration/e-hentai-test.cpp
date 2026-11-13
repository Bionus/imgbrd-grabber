#include <QJsonObject>
#include <QScopedPointer>
#include <QSettings>
#include <QSignalSpy>
#include <QTemporaryFile>
#include "custom-network-access-manager.h"
#include "filename/filename.h"
#include "loader/token.h"
#include "mixed-settings.h"
#include "models/image.h"
#include "models/image-factory.h"
#include "models/page.h"
#include "models/page-api.h"
#include "models/profile.h"
#include "models/site.h"
#include "catch.h"
#include "source-helpers.h"

namespace
{
	QByteArray galleryHtml(int start, int count, int total, const QString &layout)
	{
		QString html = QString("<html><div id=\"taglist\"></div><p data-extra='value' class='extra gpc'>\nShowing %1 - %2\n of %3 images </p>")
			.arg(start).arg(start + count - 1).arg(total);
		for (int index = 0; index < count; ++index) {
			const int position = start + index;
			const QString url = QString("https://e-hentai.org/s/token/123-%1").arg(position);
			const QString style = "background:url(https://example.org/thumb.jpg) 0px 0px; width:100px; height:100px";
			if (layout == "gdtm") {
				html += QString("<div class=\"gdtm\"><div style=\"%1\"><a href=\"%2\"><img src=\"https://example.org/thumb.jpg\"></a></div></div>").arg(style, url);
			} else if (layout == "gdtl") {
				html += QString("<div class=\"gdtl\"><a href=\"%1\"><img src=\"https://example.org/thumb.jpg\"></a></div>").arg(url);
			} else {
				html += QString("<a href=\"%1\"><div title=\"Page %2: image.jpg\" style=\"%3\"></div></a>").arg(url).arg(position).arg(style);
			}
		}
		return (html + "</html>").toUtf8();
	}
}

TEST_CASE("E-Hentai gallery filenames", "[gallery]")
{
	setupSource("E-Hentai");
	setupSite("E-Hentai", "e-hentai.org");
	const QScopedPointer<Profile> profile(makeProfile());
	auto *site = profile->getSites().value("e-hentai.org");
	REQUIRE(site != nullptr);
	site->setAutoLogin(false);
	site->settings()->setValue("download/throttle_page", 0);
	site->settings()->setValue("download/throttle_details", 0);
	REQUIRE(!site->getApis().isEmpty());

	QMap<QString, QString> galleryDetails {
		{ "type", "gallery" }, { "name", "Gallery" }, { "id", "123" },
		{ "md5", "123/token" }, { "position", "9" }, { "gallery_count", "5" },
		{ "page_url", "https://e-hentai.org/g/123/token/" },
	};
	const auto gallery = ImageFactory::build(site, galleryDetails, profile.data());
	const SearchQuery query(gallery);
	const Filename filename("%gallery.position% of %gallery.count% - %count%.jpg");

	// All three thumbnail parsers, multiple HTML pages, and deliberately different client limits.
	for (const QString &layout : { QString("gdtm"), QString("gdtl"), QString("sprite") }) {
		for (int limit : { 7, 80 }) {
			for (int pageNumber : { 1, 2, 5 }) {
				INFO("layout=" << layout.toStdString() << " limit=" << limit << " page=" << pageNumber);
				const int start = (pageNumber - 1) * 20 + 1;
				QTemporaryFile response;
				REQUIRE(response.open());
				const auto html = galleryHtml(start, 20, 100, layout);
				REQUIRE(response.write(html) == html.size());
				response.close();
				CustomNetworkAccessManager::NextFiles.enqueue(response.fileName());

				Page page(profile.data(), site, { site }, query, pageNumber, limit);
				PageApi api(&page, profile.data(), site, site->getApis().first(), query, pageNumber, limit);
				QSignalSpy loaded(&api, &PageApi::finishedLoading);
				api.load();
				if (loaded.isEmpty()) {
					REQUIRE(loaded.wait());
				}
				REQUIRE(api.errors().isEmpty());
				REQUIRE(api.images().count() == 20);
				REQUIRE(api.imagesCount() == 100);
				const auto image = api.images().at(2);
				const QString expected = QString("%1 of 100 - 7.jpg").arg(start + 2);
				REQUIRE(filename.path(*image, profile.data(), "", 7) == QStringList { expected });

				// The explicit range must not change the existing result-list position.
				REQUIRE(image->token<int>("position") == (pageNumber - 1) * api.imagesPerPage() + 3);
				QJsonObject json;
				image->write(json);
				Image restored(profile.data());
				REQUIRE(restored.read(json, profile->getSites()));
				REQUIRE(filename.path(restored, profile.data(), "", 7) == QStringList { expected });

				// Loading a new file URL must retain the gallery metadata through token refresh.
				QTemporaryFile details;
				REQUIRE(details.open());
				const QByteArray detailHtml("<img id=\"img\" src=\"https://example.org/original.jpg\">");
				REQUIRE(details.write(detailHtml) == detailHtml.size());
				details.close();
				CustomNetworkAccessManager::NextFiles.enqueue(details.fileName());
				QSignalSpy detailsLoaded(image.data(), &Image::finishedLoadingTags);
				image->loadDetails();
				if (detailsLoaded.isEmpty()) {
					REQUIRE(detailsLoaded.wait());
				}
				REQUIRE(image->url().toString() == "https://example.org/original.jpg");
				REQUIRE(filename.path(*image, profile.data(), "", 7) == QStringList { expected });
			}
		}
	}
}
