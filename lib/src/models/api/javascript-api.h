#ifndef JAVASCRIPT_API_H
#define JAVASCRIPT_API_H

#include <QJSValue>
#include <QMutex>
#include "models/api/api.h"
#include "tags/tag.h"


class Page;
class Site;

class JavascriptApi : public Api
{
	Q_OBJECT

	public:
		explicit JavascriptApi(const QMap<QString, QString> &data, const QJSValue &source, QMutex *jsEngineMutex, const QString &key);

		// API
		PageUrl pageUrl(const QString &search, int page, int limit, int lastPage, int lastPageMinId, int lastPageMaxId, Site *site) const override;
		ParsedPage parsePage(Page *parentPage, const QString &source, int first, int limit) const override;
		PageUrl tagsUrl(int page, int limit, Site *site) const override;
		ParsedTags parseTags(const QString &source, Site *site) const override;
		PageUrl detailsUrl(qulonglong id, const QString &md5, Site *site) const override;
		ParsedDetails parseDetails(const QString &source, Site *site) const override;
		PageUrl checkUrl() const override;
		ParsedCheck parseCheck(const QString &source) const override;
		bool needAuth() const override;
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

	private:
		const QJSValue &m_source;
		QString m_key;
		mutable QMutex *m_engineMutex;
};

#endif // JAVASCRIPT_API_H
