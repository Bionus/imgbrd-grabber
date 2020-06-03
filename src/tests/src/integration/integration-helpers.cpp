#include "integration-helpers.h"
#include <QSettings>
#include <QSignalSpy>
#include <QStringList>
#include "custom-network-access-manager.h"
#include "downloader/downloader.h"
#include "logger.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"
#include "tags/tag-api.h"
#include "catch.h"
#include "raii-helpers.h"
#include "source-helpers.h"


QList<QSharedPointer<Image>> getImages(const QString &source, const QString &site, const QString &format, const QString &tags, const QString &file)
{
	setupSource(source);
	setupSite(source, site);

	// Setup network
	if (!file.isEmpty()) {
		CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/" + site + "/" + file);
	}

	QSettings settings("tests/resources/sites/" + source + "/" + site + "/settings.ini", QSettings::IniFormat);
	settings.clear();
	settings.setValue("download/throttle_retry", 0);
	settings.setValue("download/throttle_page", 0);
	settings.setValue("download/throttle_thumbnail", 0);
	settings.setValue("download/throttle_details", 0);
	settings.setValue("sources/usedefault", false);
	settings.setValue("sources/source_1", format);
	settings.sync();

	FileDeleter settingsDeleter(settings.fileName());

	const QScopedPointer<Profile> pProfile(makeProfile());
	auto profile = pProfile.data();

	Source *srce = profile->getSources().value(source);
	REQUIRE(srce != nullptr);

	QList<Site*> sites;
	Site *ste = new Site(site, srce);
	ste->setAutoLogin(false);
	sites.append(ste);

	QList<QSharedPointer<Image>> result;
	const QScopedPointer<Downloader> downloader(new Downloader(profile,
		tags.split(' '),
		QStringList(),
		sites,
		1,
		20,
		20,
		".",
		"%md5%.%ext%",
		"",
		"",
		false,
		Blacklist(),
		false,
		0,
		"%tag %count %type"));
	downloader->setQuit(false);

	// Wait for downloader
	QSignalSpy spy(downloader.data(), SIGNAL(finishedImages(QList<QSharedPointer<Image>>)));
	downloader->getImages();
	if (!spy.wait()) {
		return result;
	}

	// Get results
	QList<QVariant> arguments = spy.takeFirst();
	QVariantList variants = arguments.at(0).value<QVariantList>();

	// Convert results
	result.reserve(variants.count());
	for (const QVariant &variant : variants) {
		QSharedPointer<Image> img = variant.value<QSharedPointer<Image>>();
		result.append(img);
	}
	return result;
}

QList<Tag> getPageTags(const QString &source, const QString &site, const QString &format, const QString &tags, const QString &file)
{
	setupSource(source);
	setupSite(source, site);

	QSettings settings("tests/resources/sites/" + source + "/" + site + "/settings.ini", QSettings::IniFormat);
	settings.clear();
	settings.setValue("download/throttle_retry", 0);
	settings.setValue("download/throttle_page", 0);
	settings.setValue("download/throttle_thumbnail", 0);
	settings.setValue("download/throttle_details", 0);
	settings.setValue("sources/usedefault", false);
	settings.setValue("sources/source_1", format);
	settings.sync();

	FileDeleter settingsDeleter(settings.fileName());

	// Setup network
	if (!file.isEmpty()) {
		CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/" + site + "/" + file);
	}

	const QScopedPointer<Profile> pProfile(makeProfile());
	auto profile = pProfile.data();

	Source *srce = profile->getSources().value(source);
	REQUIRE(srce != nullptr);

	QList<Site*> sites;
	Site *ste = new Site(site, srce);
	ste->setAutoLogin(false);
	sites.append(ste);

	QList<Tag> result;
	const QScopedPointer<Downloader> downloader(new Downloader(profile,
		tags.split(' '),
		QStringList(),
		sites,
		1,
		20,
		20,
		".",
		"%md5%.%ext%",
		"",
		"",
		false,
		Blacklist(),
		false,
		0,
		"%tag %count %type"));
	downloader->setQuit(false);

	// Wait for downloader
	QSignalSpy spy(downloader.data(), SIGNAL(finishedTags(QList<Tag>)));
	downloader->getPageTags();
	if (!spy.wait()) {
		return result;
	}

	// Get results
	QList<QVariant> arguments = spy.takeFirst();
	QVariantList variants = arguments.at(0).value<QVariantList>();

	// Convert results
	result.reserve(variants.count());
	for (const QVariant &variant : variants) {
		Tag tag = variant.value<Tag>();
		result.append(tag);
	}
	return result;
}

QList<Tag> getTags(const QString &source, const QString &site, const QString &format, const QString &file)
{
	setupSource(source);
	setupSite(source, site);

	QSettings settings("tests/resources/sites/" + source + "/" + site + "/settings.ini", QSettings::IniFormat);
	settings.clear();
	settings.setValue("download/throttle_retry", 0);
	settings.setValue("download/throttle_page", 0);
	settings.setValue("download/throttle_thumbnail", 0);
	settings.setValue("download/throttle_details", 0);
	settings.setValue("sources/usedefault", false);
	settings.setValue("sources/source_1", format);
	settings.sync();

	FileDeleter settingsDeleter(settings.fileName());

	// Setup network
	if (!file.isEmpty()) {
		CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/" + site + "/" + file);
	}

	const QScopedPointer<Profile> pProfile(makeProfile());
	auto profile = pProfile.data();

	Source *srce = profile->getSources().value(source);
	REQUIRE(srce != nullptr);

	Site *ste = new Site(site, srce);
	ste->setAutoLogin(false);

	QList<Tag> result;
	TagApi tagApi(profile, ste, ste->getApis().first(), 1, 100);

	// Wait for tag api
	QSignalSpy spy(&tagApi, SIGNAL(finishedLoading(TagApiBase*, TagApiBase::LoadResult)));
	tagApi.load(false);
	if (!spy.wait()) {
		return result;
	}

	// Check result type
	QList<QVariant> arguments = spy.takeFirst();
	TagApi::LoadResult res = arguments.at(1).value<TagApi::LoadResult>();
	if (res != TagApi::LoadResult::Ok) {
		return result;
	}

	return tagApi.tags();
}
