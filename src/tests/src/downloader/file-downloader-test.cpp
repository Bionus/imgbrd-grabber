#include <QFile>
#include <QSignalSpy>
#include "custom-network-access-manager.h"
#include "downloader/file-downloader.h"
#include "network/network-manager.h"
#include "catch.h"


QString fileMd5(const QString &path)
{
	QCryptographicHash hash(QCryptographicHash::Md5);

	QFile f(path);
	if (!f.open(QFile::ReadOnly)) {
		return QString();
	}

	hash.addData(&f);
	f.close();
	return hash.result().toHex();
}


TEST_CASE("FileDownloader")
{
	const QString successUrl = "https://raw.githubusercontent.com/Bionus/imgbrd-grabber/master/gui/resources/images/icon.png";
	const QString successMd5 = "005ffe0a3ffcb67fb2da4671d28fd363";
	NetworkManager accessManager;

	SECTION("Success")
	{
		CustomNetworkAccessManager::NextFiles.enqueue("gui/resources/images/icon.png");

		NetworkReply *reply = accessManager.get(QNetworkRequest(QUrl(successUrl)));
		QString dest = "single.png";

		FileDownloader downloader(false);
		QSignalSpy spy(&downloader, SIGNAL(success()));
		REQUIRE(downloader.start(reply, dest));
		REQUIRE(spy.wait());

		REQUIRE(fileMd5(dest) == successMd5);
		QFile::remove(dest);
	}

	SECTION("NetworkError")
	{
		CustomNetworkAccessManager::NextFiles.enqueue("404");

		NetworkReply *reply = accessManager.get(QNetworkRequest(QUrl("testNetworkError")));
		QString dest = "single.png";

		FileDownloader downloader(false);
		qRegisterMetaType<NetworkReply::NetworkError>("NetworkReply::NetworkError");
		QSignalSpy spy(&downloader, SIGNAL(networkError(NetworkReply::NetworkError, QString)));
		REQUIRE(downloader.start(reply, dest));
		REQUIRE(spy.wait());

		QList<QVariant> arguments = spy.takeFirst();
		auto code = arguments[0].value<NetworkReply::NetworkError>();

		REQUIRE(code == NetworkReply::NetworkError::ContentNotFoundError);
		REQUIRE(!QFile::exists(dest));
	}

	SECTION("FailedStart")
	{
		NetworkReply *reply = accessManager.get(QNetworkRequest(QUrl("testFailedStart")));
		QString dest = "////////";

		FileDownloader downloader(false);
		REQUIRE(!downloader.start(reply, dest));

		accessManager.clear();
	}

	SECTION("InvalidHtml")
	{
		CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/homepage.html");

		NetworkReply *reply = accessManager.get(QNetworkRequest(QUrl("testInvalidHtml")));
		QString dest = "test.html";

		FileDownloader downloader(false);
		qRegisterMetaType<NetworkReply::NetworkError>("NetworkReply::NetworkError");
		QSignalSpy spy(&downloader, SIGNAL(networkError(NetworkReply::NetworkError, QString)));
		REQUIRE(downloader.start(reply, dest));
		REQUIRE(spy.wait());

		QList<QVariant> arguments = spy.takeFirst();
		auto code = arguments[0].value<NetworkReply::NetworkError>();
		QString error = arguments[1].toString();

		REQUIRE(code == NetworkReply::NetworkError::ContentNotFoundError);
		REQUIRE(error == QString("Invalid HTML content returned"));
		REQUIRE(!QFile::exists(dest));
	}
}
