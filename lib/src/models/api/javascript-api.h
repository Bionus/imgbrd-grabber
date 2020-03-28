#ifndef JAVASCRIPT_API_H
#define JAVASCRIPT_API_H

#include <QJSValue>
#include "models/api/api.h"


class Page;
class QMutex;
class Site;
class Tag;

class JavascriptApi : public Api
{
	Q_OBJECT

	public:
		explicit JavascriptApi(const QJSValue &source, QMutex *jsEngineMutex, const QString &key);

		// Normal search
		PageUrl pageUrl(const QString &search, int page, int limit, int lastPage, qulonglong lastPageMinId, qulonglong lastPageMaxId, Site *site) const override;
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
		PageUrl tagsUrl(int page, int limit, Site *site) const override;
		bool parseTagsErrors() const override;
		ParsedTags parseTags(const QString &source, int statusCode, Site *site) const override;

		// Image details
		PageUrl detailsUrl(qulonglong id, const QString &md5, Site *site) const override;
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
		bool canLoadCheck() const override;
		int forcedLimit() const override;
		int maxLimit() const override;
		QStringList modifiers() const override;
		QStringList forcedTokens() const override;

	protected:
		void fillUrlObject(const QJSValue &result, Site *site, PageUrl &ret) const;
		QList<Tag> makeTags(const QJSValue &tags, Site *site) const;
		QJSValue getJsConst(const QString &key, const QJSValue &def = QJSValue(QJSValue::UndefinedValue)) const;
		ParsedPage parsePageInternal(const QString &type, Page *parentPage, const QString &source, int statusCode, int first) const;

	private:
		const QJSValue &m_source;
		QString m_key;
		mutable QMutex *m_engineMutex;
};

#endif // JAVASCRIPT_API_H
