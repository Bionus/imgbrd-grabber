#ifndef API_H
#define API_H

#include "models/image.h"
#include "tags/tag.h"

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
	bool ok;
};

class Site;

class Api : public QObject
{
	Q_OBJECT

	public:
		Api(QString name, QMap<QString, QString> data);

		// Getters
		QString getName() const;
		virtual bool needAuth() const;

		// Info getters
		bool contains(const QString &key) const;
		QString value(const QString &key) const;
		QString operator[](const QString &key) const { return value(key); }

		// API
		virtual PageUrl pageUrl(const QString &search, int page, int limit, int lastPage, int lastPageMinId, int lastPageMaxId, Site *site) const;
		virtual ParsedPage parsePage(Page *parentPage, const QString &source, int first, int limit) const = 0;
		virtual PageUrl tagsUrl(int page, int limit, Site *site) const;
		virtual ParsedTags parseTags(const QString &source, Site *site) const = 0;
		virtual PageUrl detailsUrl(qulonglong id, const QString &md5, Site *site) const;
		virtual ParsedDetails parseDetails(const QString &source, Site *site) const;
		virtual PageUrl checkUrl() const;
		virtual ParsedCheck parseCheck(const QString &source) const;
		virtual bool canLoadTags() const;
		virtual bool canLoadDetails() const;
		virtual bool canLoadCheck() const;
		virtual int forcedLimit() const;
		virtual int maxLimit() const;
		virtual QStringList modifiers() const;
		virtual QStringList forcedTokens() const;

	protected:
		QSharedPointer<Image> parseImage(Page *parentPage, QMap<QString, QString> d, int position, const QList<Tag> &tags = QList<Tag>(), bool replaces = true) const;
		QString parseSetImageUrl(Site *site, const QString &settingUrl, const QString &settingReplaces, QString ret, QMap<QString, QString> *d, bool replaces = true, const QString &def = QString()) const;

	private:
		QString m_name;
		QMap<QString, QString> m_data;
};

#endif // API_H
