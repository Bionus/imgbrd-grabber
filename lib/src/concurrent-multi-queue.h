#ifndef CONCURRENT_MULTI_QUEUE_H
#define CONCURRENT_MULTI_QUEUE_H

#include <QAtomicInt>
#include <QPair>
#include <QQueue>
#include <QList>
#include <QVariant>


class ConcurrentMultiQueue : public QObject
{
	Q_OBJECT

	public:
		ConcurrentMultiQueue(QObject *parent = nullptr);
		int globalConcurrency() const;
		void setGlobalConcurrency(int globalConcurrency);
		void append(QQueue<QVariant> queue, int priority = 1);

	public slots:
		void next();

	signals:
		void dequeued(QVariant next);
		void finished();

	private:
		int m_globalConcurrency = 1;
		QList<QPair<QQueue<QVariant>, int>> m_queues;

		QAtomicInt m_activeWorkers;
		int m_roundRobin = 0;
		int m_priority = 0;
};

#endif // CONCURRENT_MULTI_QUEUE_H
