#ifndef CONCURRENT_MULTI_QUEUE_H
#define CONCURRENT_MULTI_QUEUE_H

#include <QAtomicInt>
#include <QQueue>
#include <QVariant>
#include <QVector>


class ConcurrentMultiQueue : public QObject
{
	Q_OBJECT

	public:
		ConcurrentMultiQueue(QObject *parent = nullptr);
		int globalConcurrency() const;
		void setGlobalConcurrency(int globalConcurrency);
		void append(int queue, QVariant item);

	public slots:
		void next();

	protected slots:
		void nextInternal();

	signals:
		void dequeued(QVariant next);
		void finished();

	private:
		int m_globalConcurrency = 1;
		QVector<QQueue<QVariant>> m_queues;

		QAtomicInt m_activeWorkers;
};

#endif // CONCURRENT_MULTI_QUEUE_H
