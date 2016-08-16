#ifndef TEST_SUITE_H
#define TEST_SUITE_H

#include <QObject>
#include <QList>


class TestSuite : public QObject
{
	Q_OBJECT

	public:
		explicit TestSuite();

	public:
		static QList<QObject*> suites;

};

#endif // TEST_SUITE_H
