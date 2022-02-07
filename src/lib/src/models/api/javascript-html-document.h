#ifndef JAVASCRIPT_HTML_DOCUMENT_H
#define JAVASCRIPT_HTML_DOCUMENT_H

#include <QJSValue>
#include <QObject>
#include "lexbor/html/html.h"


class QJSEngine;

class JavascriptHtmlDocument : public QObject
{
	Q_OBJECT

	public:
		JavascriptHtmlDocument(QJSEngine &engine, lxb_dom_node_t *node);
		static JavascriptHtmlDocument *fromString(QJSEngine &m_engine, const QString &html);

		Q_INVOKABLE QJSValue innerHTML() const;
		Q_INVOKABLE QJSValue find(const QString &css) const;

	private:
		QJSEngine &m_engine;
		lxb_dom_node_t *m_document;
};

#endif // JAVASCRIPT_HTML_DOCUMENT_H
