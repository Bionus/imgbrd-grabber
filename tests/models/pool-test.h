#ifndef POOL_TEST_H
#define POOL_TEST_H

#include "test-suite.h"
#include "models/pool.h"


class PoolTest : public TestSuite
{
	Q_OBJECT

    private slots:
        void testGetId();
        void testGetName();
        void testGetCurrent();
        void testGetNext();
        void testGetPrevious();
};

#endif // POOL_TEST_H
