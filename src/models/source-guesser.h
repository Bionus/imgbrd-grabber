#ifndef SOURCE_GUESSER_H
#define SOURCE_GUESSER_H

#include "custom-network-access-manager.h"
#include "models/source.h"


class SourceGuesser : public QObject
{
	Q_OBJECT

	public:
		SourceGuesser(QString url, QList<Source*> sources);
		Source *start();

	signals:
		void progress(int current);
		void finished(Source *result);

	private:
		QString m_url;
		QList<Source*> m_sources;
		CustomNetworkAccessManager *m_manager;
		QMap<QString, QString> m_cache;
};

#endif // SOURCE_GUESSER_H
