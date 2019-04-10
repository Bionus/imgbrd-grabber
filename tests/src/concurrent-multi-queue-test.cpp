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

	QSignalSpy spy(&multiQueue, &ConcurrentMultiQueue::finished);
	multiQueue.append(makeQueue({ 1, 2, 3 }));

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

	QSignalSpy spy(&multiQueue, &ConcurrentMultiQueue::finished);
	multiQueue.append(makeQueue({ 1, 2, 3 }));
	multiQueue.append(makeQueue({ 4, 5 }));
	multiQueue.append(makeQueue({ 6, 7, 8, 9 }));

	if (!spy.wait()) {
		QFAIL("finished signal not received");
	}

	QCOMPARE(results, QList<int>() << 1 << 4 << 6 << 2 << 5 << 7 << 3 << 8 << 9);
}

void ConcurrentMultiQueueTest::multipleQueuesWithPriority()
{
	QList<int> results;
	ConcurrentMultiQueue multiQueue;
	connect(&multiQueue, &ConcurrentMultiQueue::dequeued, [&](QVariant item) {
		results.append(item.toInt());
		multiQueue.next();
	});

	QSignalSpy spy(&multiQueue, &ConcurrentMultiQueue::finished);
	multiQueue.append(makeQueue({ 1, 2, 3 }), 2);
	multiQueue.append(makeQueue({ 4, 5 }), 1);
	multiQueue.append(makeQueue({ 6, 7, 8, 9 }), 100);

	if (!spy.wait()) {
		QFAIL("finished signal not received");
	}

	QCOMPARE(results, QList<int>() << 1 << 2 << 4 << 6 << 7 << 8 << 9 << 3 << 5);
}


QTEST_MAIN(ConcurrentMultiQueueTest)
