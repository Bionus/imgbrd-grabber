#include "test-suite.h"


QList<QObject*> TestSuite::suites;

TestSuite::TestSuite()
	: QObject(), profile("tests/resources/")
{
	suites.append(this);
}
