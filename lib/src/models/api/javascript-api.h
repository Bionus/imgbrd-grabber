#ifndef JAVASCRIPT_API_H
#define JAVASCRIPT_API_H

#include "models/api/api.h"
#include <QJSValue>


class JavascriptApi : public Api
{
	Q_OBJECT

	public:
		explicit JavascriptApi(const QJSValue &source, const QString &key);
		ParsedPage parsePage(Page *parentPage, const QString &source, int first) const override;

	private:
		QList<Tag> makeTags(const QJSValue &tags) const;
		const QJSValue &m_source;
		QString m_key;
};

#endif // JAVASCRIPT_API_H
