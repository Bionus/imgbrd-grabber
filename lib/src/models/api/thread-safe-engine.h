#ifndef THREAD_SAFE_ENGINE_H
#define THREAD_SAFE_ENGINE_H

#include <QMutexLocker>


class QJSEngine;
class QMutex;

class ThreadSafeEngine
{
	public:
		ThreadSafeEngine(QJSEngine *engine, QMutex *mutex);
		~ThreadSafeEngine();
		QJSEngine *engine;

	private:
		QMutexLocker m_mutexLocker;
};

#endif // THREAD_SAFE_ENGINE_H
