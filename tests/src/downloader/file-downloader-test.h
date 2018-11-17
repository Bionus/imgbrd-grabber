#ifndef FILE_DOWNLOADER_TEST_H
#define FILE_DOWNLOADER_TEST_H

#include <QNetworkAccessManager>
#include <QString>
#include "test-suite.h"


class FileDownloaderTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testSuccessSingle();
		void testSuccessMultiple();
		void testNetworkError();
		void testFailedStart();
		void testInvalidHtml();

	private:
		QString m_successUrl = "https://raw.githubusercontent.com/Bionus/imgbrd-grabber/master/gui/resources/images/icon.png";
		QString m_successMd5 = "005ffe0a3ffcb67fb2da4671d28fd363";
		QNetworkAccessManager m_accessManager;
};

#endif // FILE_DOWNLOADER_TEST_H
