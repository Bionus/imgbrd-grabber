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


void ConcurrentMultiQueue::append(QQueue<QVariant> queue, int priority)
{
    if (!queue.isEmpty()) {
        m_queues.append({ queue, priority });
    }

    if (m_queues.isEmpty()) {
		QTimer::singleShot(0, this, SIGNAL(finished()));
    }

    int activeWorkers = m_activeWorkers.fetchAndStoreRelaxed(m_globalConcurrency);
    for (int i = activeWorkers; i < m_globalConcurrency; ++i) {
		QTimer::singleShot(0, this, SLOT(next()));
    }
}

void ConcurrentMultiQueue::next()
{
    if (m_queues.isEmpty()) {
        int activeWorkers = m_activeWorkers.fetchAndAddRelaxed(-1);
        if (activeWorkers == 1) { // Compare to 1 because the returned value is the one BEFORE the modification
            emit finished();
        }
        return;
    }

	QVariant next = m_queues[m_roundRobin].first.dequeue();
    if (m_queues[m_roundRobin].first.isEmpty()) {
        m_queues.removeAt(m_roundRobin);
		m_priority = 0;
    } else {
        m_priority++;
        if (m_priority >= m_queues[m_roundRobin].second) {
			m_roundRobin++;
        }
    }
	if (!m_queues.isEmpty()) {
		m_roundRobin = m_roundRobin % m_queues.count();
	}

    emit dequeued(next);
}
