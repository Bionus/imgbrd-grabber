#include "models/api/thread-safe-engine.h"


ThreadSafeEngine::ThreadSafeEngine(QJSEngine *_engine, QMutex *mutex)
	: engine(_engine), m_mutexLocker(mutex)
{}
