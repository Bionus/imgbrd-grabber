#include "file-downloader-test.h"
#include <QtTest>
#include "custom-network-access-manager.h"
#include "downloader/file-downloader.h"
#include "network/network-manager.h"


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


void FileDownloaderTest::testSuccessSingle()
{
	CustomNetworkAccessManager::NextFiles.enqueue("gui/resources/images/icon.png");

	NetworkReply *reply = m_accessManager.get(QNetworkRequest(QUrl(m_successUrl)));
	QString dest = "single.png";

	FileDownloader downloader(false);
	QSignalSpy spy(&downloader, SIGNAL(success()));
	QVERIFY(downloader.start(reply, dest));
	QVERIFY(spy.wait());

	QCOMPARE(fileMd5(dest), m_successMd5);
	QFile::remove(dest);
}

void FileDownloaderTest::testSuccessMultiple()
{
	CustomNetworkAccessManager::NextFiles.enqueue("gui/resources/images/icon.png");

	NetworkReply *reply = m_accessManager.get(QNetworkRequest(QUrl(m_successUrl)));
	QStringList dest = QStringList() << "multiple-1.png" << "multiple-2.png" << "multiple-3.png";

	FileDownloader downloader(false);
	QSignalSpy spy(&downloader, SIGNAL(success()));
	QVERIFY(downloader.start(reply, dest));
	QVERIFY(spy.wait());

	for (const QString &path : dest) {
		QCOMPARE(fileMd5(path), m_successMd5);
		QFile::remove(path);
	}
}

void FileDownloaderTest::testNetworkError()
{
	CustomNetworkAccessManager::NextFiles.enqueue("404");

	NetworkReply *reply = m_accessManager.get(QNetworkRequest(QUrl("testNetworkError")));
	QString dest = "single.png";

	FileDownloader downloader(false);
	qRegisterMetaType<NetworkReply::NetworkError>("NetworkReply::NetworkError");
	QSignalSpy spy(&downloader, SIGNAL(networkError(NetworkReply::NetworkError, QString)));
	QVERIFY(downloader.start(reply, dest));
	QVERIFY(spy.wait());

	QList<QVariant> arguments = spy.takeFirst();
	NetworkReply::NetworkError code = arguments[0].value<NetworkReply::NetworkError>();

	QCOMPARE(code, NetworkReply::NetworkError::ContentNotFoundError);
	QVERIFY(!QFile::exists(dest));
}

void FileDownloaderTest::testFailedStart()
{
	NetworkReply *reply = m_accessManager.get(QNetworkRequest(QUrl("testFailedStart")));
	QString dest = "////////";

	FileDownloader downloader(false);
	QVERIFY(!downloader.start(reply, dest));

	m_accessManager.clear();
}

void FileDownloaderTest::testInvalidHtml()
{
	CustomNetworkAccessManager::NextFiles.enqueue("tests/resources/pages/danbooru.donmai.us/homepage.html");

	NetworkReply *reply = m_accessManager.get(QNetworkRequest(QUrl("testInvalidHtml")));
	QString dest = "test.html";

	FileDownloader downloader(false);
	qRegisterMetaType<NetworkReply::NetworkError>("NetworkReply::NetworkError");
	QSignalSpy spy(&downloader, SIGNAL(networkError(NetworkReply::NetworkError, QString)));
	QVERIFY(downloader.start(reply, dest));
	QVERIFY(spy.wait());

	QList<QVariant> arguments = spy.takeFirst();
	NetworkReply::NetworkError code = arguments[0].value<NetworkReply::NetworkError>();
	QString error = arguments[1].toString();

	QCOMPARE(code, NetworkReply::NetworkError::ContentNotFoundError);
	QCOMPARE(error, QString("Invalid HTML content returned"));
	QVERIFY(!QFile::exists(dest));
}


QTEST_MAIN(FileDownloaderTest)
