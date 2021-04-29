#ifndef TAG_SEARCH_LOADER_H
#define TAG_SEARCH_LOADER_H

#include "search-loader.h"
#include <QString>


class TagSearchLoader : public SearchLoader
{
	Q_OBJECT

	Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)

	public:
		explicit TagSearchLoader(QObject *parent = nullptr);

		const QString &query() const;
		void setQuery(const QString &query);

	public slots:
		void load() override;

	signals:
		void queryChanged();

	private:
		QString m_query;
};

#endif // TAG_SEARCH_LOADER_H
