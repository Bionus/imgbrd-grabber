#include "concurrent-multi-queue.h"
#include <QTimer>


ConcurrentMultiQueue::ConcurrentMultiQueue(QObject *parent)
	: QObject(parent)
{}


int ConcurrentMultiQueue::globalConcurrency() const
{
    return m_globalConcurrency;
}

void ConcurrentMultiQueue::setGlobalConcurrency(int globalConcurrency)
{
    m_globalConcurrency = globalConcurrency;
}


void ConcurrentMultiQueue::append(int queue, QVariant item)
{
	if (queue >= m_queues.count()) {
		m_queues.resize(queue + 1);
	}

	m_queues[queue].append(item);

	if (m_activeWorkers.load() < m_globalConcurrency) {
		m_activeWorkers.fetchAndAddRelaxed(1);
		QTimer::singleShot(0, this, SLOT(next()));
	}
}

void ConcurrentMultiQueue::next()
{
	int index = 0;
	while (index < m_queues.count() && m_queues[index].isEmpty()) {
		index++;
	}

	if (index >= m_queues.count()) {
        int activeWorkers = m_activeWorkers.fetchAndAddRelaxed(-1);
        if (activeWorkers == 1) { // Compare to 1 because the returned value is the one BEFORE the modification
            emit finished();
        }
        return;
    }

	QVariant next = m_queues[index].dequeue();
    emit dequeued(next);
}
