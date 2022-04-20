#include "source-engine.h"
#include <QString>
#include "api/api.h"


SourceEngine::SourceEngine(QObject *parent)
	: QObject(parent)
{}

Api *SourceEngine::getApi(const QString &name) const
{
	for (Api *api : this->getApis()) {
		if (api->getName() == name) {
			return api;
		}
	}
	return nullptr;
}
