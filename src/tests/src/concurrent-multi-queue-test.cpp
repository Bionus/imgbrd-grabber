#include <QList>
#include <QQueue>
#include <QSignalSpy>
#include "concurrent-multi-queue.h"
#include "catch.h"


TEST_CASE("ConcurrentMultiQueue")
{
	SECTION("Single queue")
	{
		QList<int> results;
		ConcurrentMultiQueue multiQueue;
		QObject::connect(&multiQueue, &ConcurrentMultiQueue::dequeued, [&](QVariant item) {
			results.append(item.toInt());
			multiQueue.next();
		});

		QSignalSpy spy(&multiQueue, SIGNAL(finished()));
		multiQueue.append(0, 1);
		multiQueue.append(0, 2);
		multiQueue.append(0, 3);
		REQUIRE(spy.wait());

		REQUIRE(results == QList<int>() << 1 << 2 << 3);
	}

	SECTION("Multiple queues")
	{
		QList<int> results;
		ConcurrentMultiQueue multiQueue;
		QObject::connect(&multiQueue, &ConcurrentMultiQueue::dequeued, [&](QVariant item) {
			results.append(item.toInt());
			multiQueue.next();
		});

		QSignalSpy spy(&multiQueue, SIGNAL(finished()));
		multiQueue.append(2, 1);
		multiQueue.append(1, 2);
		multiQueue.append(2, 3);
		multiQueue.append(0, 4);
		multiQueue.append(0, 5);
		multiQueue.append(1, 6);
		REQUIRE(spy.wait());

		REQUIRE(results == QList<int>() << 4 << 5 << 2 << 6 << 1 << 3);
	}
}
