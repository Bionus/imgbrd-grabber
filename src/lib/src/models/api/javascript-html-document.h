#ifndef JAVASCRIPT_HTML_DOCUMENT_H
#define JAVASCRIPT_HTML_DOCUMENT_H

#include <QJSValue>
#include <QObject>
#include "utils/html-node.h"


class QJSEngine;

class JavascriptHtmlDocument : public QObject
{
	Q_OBJECT

	public:
		JavascriptHtmlDocument(QJSEngine &engine, const HtmlNode &node);
		static JavascriptHtmlDocument *fromString(QJSEngine &m_engine, const QString &html, bool fragment = false);

		Q_INVOKABLE QJSValue outerHTML() const;
		Q_INVOKABLE QJSValue innerHTML() const;
		Q_INVOKABLE QJSValue innerText() const;

		Q_INVOKABLE QJSValue tag() const;
		Q_INVOKABLE QJSValue attr(const QString &attr) const;
		Q_INVOKABLE QJSValue path() const;
		Q_INVOKABLE QJSValue pathIds() const;

		Q_INVOKABLE QJSValue find(const QString &css) const;

	private:
		QJSEngine &m_engine;
		HtmlNode m_node;
};

#endif // JAVASCRIPT_HTML_DOCUMENT_H
