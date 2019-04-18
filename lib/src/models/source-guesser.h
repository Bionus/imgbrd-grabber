#ifndef SOURCE_GUESSER_H
#define SOURCE_GUESSER_H

#include <QList>
#include <QMap>
#include <QObject>
#include <QString>


class NetworkManager;
class Source;

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
		NetworkManager *m_manager;
		QMap<QString, QString> m_cache;
};

#endif // SOURCE_GUESSER_H
