#ifndef JAVASCRIPT_API_H
#define JAVASCRIPT_API_H

#include <QJSValue>
#include <QString>
#include "models/api/api.h"
#include "models/page-information.h"


class Page;
class QJSEngine;
class QMutex;
class Site;
class Tag;

class JavascriptApi : public Api
{
	Q_OBJECT

	public:
		explicit JavascriptApi(QJSEngine *engine, const QJSValue &source, QMutex *jsEngineMutex, const QString &key);

		// Normal search
		PageUrl pageUrl(const QString &search, int page, int limit, const PageInformation &lastPage, Site *site) const override;
		bool parsePageErrors() const override;
		ParsedPage parsePage(Page *parentPage, const QString &source, int statusCode, int first) const override;

		// Gallery
		PageUrl galleryUrl(const QSharedPointer<Image> &gallery, int page, int limit, Site *site) const override;
		bool parseGalleryErrors() const override;
		ParsedPage parseGallery(Page *parentPage, const QString &source, int statusCode, int first) const override;

		// Tag types
		PageUrl tagTypesUrl(Site *site) const override;
		bool parseTagTypesErrors() const override;
		ParsedTagTypes parseTagTypes(const QString &source, int statusCode, Site *site) const override;

		// Tags
		PageUrl tagsUrl(int page, int limit, const QString &order, Site *site) const override;
		bool parseTagsErrors() const override;
		ParsedTags parseTags(const QString &source, int statusCode, Site *site) const override;

		// Image details
		PageUrl detailsUrl(qulonglong id, const QString &md5, Site *site, QVariantMap identity = {}) const override;
		bool parseDetailsErrors() const override;
		ParsedDetails parseDetails(const QString &source, int statusCode, Site *site) const override;

		// Check
		PageUrl checkUrl() const override;
		bool parseCheckErrors() const override;
		ParsedCheck parseCheck(const QString &source, int statusCode) const override;

		bool needAuth() const override;
		bool mustLoadTagTypes() const override;
		bool canLoadTagTypes() const override;
		bool canLoadTags() const override;
		bool canLoadDetails() const override;
		bool canLoadFullDetails() const override;
		bool canLoadCheck() const override;
		int forcedLimit() const override;
		int maxLimit() const override;
		QStringList modifiers() const override;
		QStringList forcedTokens() const override;
		QMap<QString, ApiEndpoint*> endpoints() const override;

		// TODO: make those protected again
		void fillUrlObject(const QJSValue &result, Site *site, PageUrl &ret) const;
		QList<Tag> makeTags(const QJSValue &tags, Site *site) const;
		QSharedPointer<Image> makeImage(const QJSValue &raw, Site *site, Page *parentPage = nullptr, int index = 0, int first = 1) const;
		QJSValue getJsConst(const QString &key, const QJSValue &def = QJSValue::UndefinedValue) const;
		ParsedPage parsePageInternal(const QString &type, Page *parentPage, const QString &source, int statusCode, int first) const;

	private:
		QJSEngine *m_engine;
		const QJSValue &m_source;
		QString m_key;
		mutable QMutex *m_engineMutex;
		QMap<QString, ApiEndpoint*> m_endpoints;
};

#endif // JAVASCRIPT_API_H
