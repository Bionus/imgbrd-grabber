#include "models/api/javascript-html-document.h"
#include <QJSEngine>
#include <QJSValue>
#include "lexbor/css/css.h"
#include "lexbor/html/html.h"
#include "lexbor/selectors/selectors.h"
#include "logger.h"


JavascriptHtmlDocument::JavascriptHtmlDocument(QJSEngine &engine, const HtmlNode &node)
	: QObject(&engine), m_engine(engine), m_node(node)
{}

JavascriptHtmlDocument *JavascriptHtmlDocument::fromString(QJSEngine &engine, const QString &html)
{
	HtmlNode *node = HtmlNode::fromString(html);
	if (node == nullptr) {
		return nullptr;
	}
	return new JavascriptHtmlDocument(engine, *node);
}


QJSValue JavascriptHtmlDocument::outerHTML() const
{
	return m_node.outerHTML();
}

QJSValue JavascriptHtmlDocument::innerHTML() const
{
	return m_node.innerHTML();
}

QJSValue JavascriptHtmlDocument::innerText() const
{
	return m_node.innerText();
}

QJSValue JavascriptHtmlDocument::attr(const QString &attr) const
{
	return m_node.attr(attr);
}

QJSValue JavascriptHtmlDocument::find(const QString &css) const
{
	const QList<HtmlNode> nodes = m_node.find(css);

	// Convert result to QJSValue
	QJSValue js = m_engine.newArray(nodes.length());
	for (int i = 0; i < nodes.length(); ++i) {
		auto *obj = new JavascriptHtmlDocument(m_engine, nodes[i]);
		js.setProperty(i, m_engine.newQObject(obj));
	}
	return js;
}
