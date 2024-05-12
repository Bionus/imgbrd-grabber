#ifndef TAG_SEARCH_LOADER_H
#define TAG_SEARCH_LOADER_H

#include "search-loader.h"
#include <QString>


class TagSearchLoader : public SearchLoader
{
	Q_OBJECT

	Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)
	Q_PROPERTY(QString endpoint READ endpoint WRITE setEndpoint NOTIFY endpointChanged)

	public:
		explicit TagSearchLoader(QObject *parent = nullptr);

		const QString &query() const;
		void setQuery(const QString &query);

		const QString &endpoint() const { return m_endpoint; }
		void setEndpoint(const QString &endpoint) { m_endpoint = endpoint; emit endpointChanged(); }

	public slots:
		void load() override;

	signals:
		void queryChanged();
		void endpointChanged();

	private:
		QString m_query;
		QString m_endpoint;
};

#endif // TAG_SEARCH_LOADER_H
