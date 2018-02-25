#ifndef API_H
#define API_H

#include <QMap>
#include <QObject>
#include <QString>
#include "models/image.h"
#include "tags/tag.h"

struct ParsedPage
{
	QString error;
	int pageCount = -1;
	int imageCount = -1;
	QList<Tag> tags;
	QList<QSharedPointer<Image>> images;
	QUrl urlNextPage;
	QUrl urlPrevPage;
	QString wiki;
};

class Site;

class Api : public QObject
{
	Q_OBJECT

	public:
		Api(const QString &name, const QMap<QString, QString> &data);

		// Getters
		QString getName() const;
		bool needAuth() const;

		// Info getters
		bool contains(const QString &key) const;
		QString value(const QString &key) const;
		QString operator[](const QString &key) const { return value(key); }

		// API
		virtual QString pageUrl(const QString &search, int page, int limit, int lastPage, int lastPageMinId, int lastPageMaxId, Site *site) const;
		virtual ParsedPage parsePage(Page *parentPage, const QString &source, int first, int limit) const = 0;
		virtual int forcedLimit() const;
		virtual int maxLimit() const;

	protected:
		QSharedPointer<Image> parseImage(Page *parentPage, QMap<QString, QString> d, int position, const QList<Tag> &tags = QList<Tag>()) const;

	private:
		QString m_name;
		QMap<QString, QString> m_data;
};

#endif // API_H
