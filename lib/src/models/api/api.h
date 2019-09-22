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
	QMap<QString, QString> headers;
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

	protected:
		explicit Api(QString name);

	public:
		// Getters
		QString getName() const;
		virtual bool needAuth() const = 0;

		// API
		virtual PageUrl pageUrl(const QString &search, int page, int limit, int lastPage, qulonglong lastPageMinId, qulonglong lastPageMaxId, Site *site) const = 0;
		virtual bool parsePageErrors() const = 0;
		virtual ParsedPage parsePage(Page *parentPage, const QString &source, int statusCode, int first) const = 0;
		virtual PageUrl galleryUrl(const QSharedPointer<Image> &gallery, int page, int limit, Site *site) const = 0;
		virtual bool parseGalleryErrors() const = 0;
		virtual ParsedPage parseGallery(Page *parentPage, const QString &source, int statusCode, int first) const = 0;
		virtual PageUrl tagsUrl(int page, int limit, Site *site) const = 0;
		virtual bool parseTagsErrors() const = 0;
		virtual ParsedTags parseTags(const QString &source, int statusCode, Site *site) const = 0;
		virtual PageUrl detailsUrl(qulonglong id, const QString &md5, Site *site) const = 0;
		virtual bool parseDetailsErrors() const = 0;
		virtual ParsedDetails parseDetails(const QString &source, int statusCode, Site *site) const = 0;
		virtual PageUrl checkUrl() const = 0;
		virtual bool parseCheckErrors() const = 0;
		virtual ParsedCheck parseCheck(const QString &source, int statusCode) const = 0;
		virtual bool canLoadTags() const = 0;
		virtual bool canLoadDetails() const = 0;
		virtual bool canLoadCheck() const = 0;
		virtual int forcedLimit() const = 0;
		virtual int maxLimit() const = 0;
		virtual QStringList modifiers() const = 0;
		virtual QStringList forcedTokens() const = 0;

	protected:
		QSharedPointer<Image> parseImage(Page *parentPage, QMap<QString, QString> d, QVariantMap data, int position, const QList<Tag> &tags = QList<Tag>()) const;

	private:
		QString m_name;
};

#endif // API_H
