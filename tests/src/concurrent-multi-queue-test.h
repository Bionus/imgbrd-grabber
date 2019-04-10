#ifndef CONCURRENT_MULTI_QUEUE_TEST_H
#define CONCURRENT_MULTI_QUEUE_TEST_H

#include "test-suite.h"


class ConcurrentMultiQueueTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void singleQueue();
		void multipleQueues();
		void multipleQueuesWithPriority();
};

#endif // CONCURRENT_MULTI_QUEUE_TEST_H
