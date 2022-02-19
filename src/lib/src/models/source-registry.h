#ifndef SOURCE_REGISTRY_H
#define SOURCE_REGISTRY_H

#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>


struct SourceRegistrySourceCommit
{
	QString hash;
	QString author;
	QString date;
	QString subject;
};

struct SourceRegistrySource
{
	QString name;
	QString hash;
	SourceRegistrySourceCommit lastCommit;
	QStringList defaultSites;
	QStringList supportedSites;
};


class NetworkManager;

class SourceRegistry : public QObject
{
	Q_OBJECT

	public:
		explicit SourceRegistry(QString jsonUrl);
		void load();

		const QString &jsonUrl() const { return m_jsonUrl; }
		const QString &name() const { return m_name; }
		const QString &home() const { return m_home; }
		const QString &url() const { return m_url; }
		const QMap<QString, SourceRegistrySource> &sources() const { return m_sources; }

	protected slots:
		void jsonLoaded();

	signals:
		void loaded();

	private:
		QString m_jsonUrl;
		NetworkManager *m_manager;

		QString m_name;
		QString m_home;
		QString m_url;
		QMap<QString, SourceRegistrySource> m_sources;
};

#endif // SOURCE_REGISTRY_H
