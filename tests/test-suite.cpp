#include "test-suite.h"


QList<QObject*> TestSuite::suites;

TestSuite::TestSuite() : QObject()
{
    suites.append(this);
}
