#include "file-downloader-test.h"
#include <QtTest>


QString fileMd5(const QString &path)
{
	QCryptographicHash hash(QCryptographicHash::Md5);

	QFile f(path);
	if (!f.open(QFile::ReadOnly))
		return QString();

	hash.addData(&f);
	f.close();
	return hash.result().toHex();
}


void FileDownloaderTest::testSuccessSingle()
{
	QNetworkReply *reply = m_accessManager.get(QNetworkRequest(QUrl(m_successUrl)));
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
	QNetworkReply *reply = m_accessManager.get(QNetworkRequest(QUrl(m_successUrl)));
	QStringList dest = QStringList() << "multiple-1.png" << "multiple-2.png" << "multiple-3.png";

	FileDownloader downloader(false);
	QSignalSpy spy(&downloader, SIGNAL(success()));
	QVERIFY(downloader.start(reply, dest));
	QVERIFY(spy.wait());

	for (const QString &path : dest)
	{
		QCOMPARE(fileMd5(path), m_successMd5);
		QFile::remove(path);
	}
}

void FileDownloaderTest::testNetworkError()
{
	QNetworkReply *reply = m_accessManager.get(QNetworkRequest(QUrl("fail://error")));
	QString dest = "single.png";

	FileDownloader downloader(false);
	QSignalSpy spy(&downloader, SIGNAL(networkError(QNetworkReply::NetworkError, QString)));
	QVERIFY(downloader.start(reply, dest));
	QVERIFY(spy.wait());

	QList<QVariant> arguments = spy.takeFirst();
	QNetworkReply::NetworkError code = arguments[0].value<QNetworkReply::NetworkError>();
	QString error = arguments[1].toString();

	QCOMPARE(code, QNetworkReply::ProtocolUnknownError);
	QCOMPARE(error, QString("Protocol \"fail\" is unknown"));
	QVERIFY(!QFile::exists(dest));
}

void FileDownloaderTest::testFailedStart()
{
	QNetworkReply *reply = m_accessManager.get(QNetworkRequest(QUrl("fail://error")));
	QString dest = "////////";

	FileDownloader downloader(false);
	QVERIFY(!downloader.start(reply, dest));
}

void FileDownloaderTest::testInvalidHtml()
{
	QNetworkReply *reply = m_accessManager.get(QNetworkRequest(QUrl(PROJECT_WEBSITE_URL)));
	QString dest = "test.html";

	FileDownloader downloader(false);
	QSignalSpy spy(&downloader, SIGNAL(networkError(QNetworkReply::NetworkError, QString)));
	QVERIFY(downloader.start(reply, dest));
	QVERIFY(spy.wait());

	QList<QVariant> arguments = spy.takeFirst();
	QNetworkReply::NetworkError code = arguments[0].value<QNetworkReply::NetworkError>();
	QString error = arguments[1].toString();

	QCOMPARE(code, QNetworkReply::ContentNotFoundError);
	QCOMPARE(error, QString("Invalid HTML content returned"));
	QVERIFY(!QFile::exists(dest));
}


QTEST_MAIN(FileDownloaderTest)
