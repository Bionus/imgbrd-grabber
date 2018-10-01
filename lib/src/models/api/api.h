#ifndef API_H
#define API_H

#include <QDateTime>
#include <QList>
#include <QMap>
#include <QSharedPointer>
#include <QUrl>
#include "tags/tag.h"


class Image;
class Page;
class Pool;
class Site;


struct PageUrl
{
	QString error;
	QString url;
};

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

struct ParsedTags
{
	QString error;
	QList<Tag> tags;
};

struct ParsedDetails
{
	QString error;
	QList<Pool> pools;
	QList<Tag> tags;
	QString imageUrl;
	QDateTime createdAt;
};

struct ParsedCheck
{
	QString error;
	bool ok = false;
};


class Api : public QObject
{
	Q_OBJECT

	public:
		Api(QString name);

		// Getters
		QString getName() const;
		virtual bool needAuth() const = 0;

		// API
		virtual PageUrl pageUrl(const QString &search, int page, int limit, int lastPage, int lastPageMinId, int lastPageMaxId, Site *site) const = 0;
		virtual ParsedPage parsePage(Page *parentPage, const QString &source, int first) const = 0;
		virtual PageUrl galleryUrl(const QString &id, int page, int limit, Site *site) const = 0;
		virtual ParsedPage parseGallery(Page *parentPage, const QString &source, int first) const = 0;
		virtual PageUrl tagsUrl(int page, int limit, Site *site) const = 0;
		virtual ParsedTags parseTags(const QString &source, Site *site) const = 0;
		virtual PageUrl detailsUrl(qulonglong id, const QString &md5, Site *site) const = 0;
		virtual ParsedDetails parseDetails(const QString &source, Site *site) const = 0;
		virtual PageUrl checkUrl() const = 0;
		virtual ParsedCheck parseCheck(const QString &source) const = 0;
		virtual bool canLoadTags() const = 0;
		virtual bool canLoadDetails() const = 0;
		virtual bool canLoadCheck() const = 0;
		virtual int forcedLimit() const = 0;
		virtual int maxLimit() const = 0;
		virtual QStringList modifiers() const = 0;
		virtual QStringList forcedTokens() const = 0;

	protected:
		QSharedPointer<Image> parseImage(Page *parentPage, QMap<QString, QString> d, int position, const QList<Tag> &tags = QList<Tag>()) const;

	private:
		QString m_name;
};

#endif // API_H
