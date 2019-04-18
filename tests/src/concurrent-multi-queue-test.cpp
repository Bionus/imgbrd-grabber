#include "concurrent-multi-queue-test.h"
#include <QList>
#include <QQueue>
#include <QSignalSpy>
#include <QtTest>
#include "concurrent-multi-queue.h"


QQueue<QVariant> makeQueue(QList<QVariant> list)
{
	QQueue<QVariant> q;
	for (const auto &val : list) {
		q.enqueue(val);
	}
	return q;
}


void ConcurrentMultiQueueTest::singleQueue()
{
	QList<int> results;
	ConcurrentMultiQueue multiQueue;
	connect(&multiQueue, &ConcurrentMultiQueue::dequeued, [&](QVariant item) {
		results.append(item.toInt());
		multiQueue.next();
	});

	QSignalSpy spy(&multiQueue, SIGNAL(finished()));
	multiQueue.append(0, 1);
	multiQueue.append(0, 2);
	multiQueue.append(0, 3);

	if (!spy.wait()) {
		QFAIL("finished signal not received");
	}

	QCOMPARE(results, QList<int>() << 1 << 2 << 3);
}

void ConcurrentMultiQueueTest::multipleQueues()
{
	QList<int> results;
	ConcurrentMultiQueue multiQueue;
	connect(&multiQueue, &ConcurrentMultiQueue::dequeued, [&](QVariant item) {
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

	if (!spy.wait()) {
		QFAIL("finished signal not received");
	}

	QCOMPARE(results, QList<int>() << 4 << 5 << 2 << 6 << 1 << 3);
}


QTEST_MAIN(ConcurrentMultiQueueTest)
