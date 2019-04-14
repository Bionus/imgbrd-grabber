#ifndef TEST_SUITE_H
#define TEST_SUITE_H

#include <QList>
#include <QObject>
#include <QString>


#include <QTest> // Necessary include to shadow QTEST_MAIN
#define QTEST_MAIN(CLASS_NAME) static CLASS_NAME instance;


class Profile;

class TestSuite : public QObject
{
	Q_OBJECT

	public:
		TestSuite();
		Profile *makeProfile() const;
		void setupSource(const QString &site, QString dir = QString()) const;
		void setupSite(const QString &site, const QString &source, QString dir = QString()) const;
		static QList<TestSuite*> &getSuites();
};

#endif // TEST_SUITE_H
