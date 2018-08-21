#ifndef TEST_SUITE_H
#define TEST_SUITE_H

#include <QList>
#include <QObject>
#include <QString>


#include <QTest> // Necessary include to shadow QTEST_MAIN
#define QTEST_MAIN(CLASS_NAME) static CLASS_NAME instance;


class TestSuite : public QObject
{
	Q_OBJECT

	public:
		TestSuite();
		void setupSource(const QString &site, QString dir = QString());
		void setupSite(const QString &site, const QString &source, QString dir = QString());
		static QList<TestSuite*> &getSuites();
};

#endif // TEST_SUITE_H
