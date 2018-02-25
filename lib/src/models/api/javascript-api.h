#ifndef JAVASCRIPT_API_H
#define JAVASCRIPT_API_H

#include <QJSValue>
#include "models/api/api.h"


class Site;

class JavascriptApi : public Api
{
	Q_OBJECT

	public:
		explicit JavascriptApi(const QMap<QString, QString> &data, const QJSValue &source, const QString &key);

		// API
		QString pageUrl(const QString &search, int page, int limit, int lastPage, int lastPageMinId, int lastPageMaxId, Site *site) const override;
		ParsedPage parsePage(Page *parentPage, const QString &source, int first, int limit) const override;
		int forcedLimit() const override;
		int maxLimit() const override;

	protected:
		QList<Tag> makeTags(const QJSValue &tags) const;
		QJSValue getJsConst(const QString &key, const QJSValue &def = QJSValue(QJSValue::UndefinedValue)) const;

	private:
		const QJSValue &m_source;
		QString m_key;
};

#endif // JAVASCRIPT_API_H
