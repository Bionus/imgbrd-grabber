#include "models/api/javascript-html-document.h"
#include <QJSEngine>
#include <QJSValue>
#include "lexbor/css/css.h"
#include "lexbor/html/html.h"
#include "lexbor/selectors/selectors.h"
#include "logger.h"


JavascriptHtmlDocument::JavascriptHtmlDocument(QJSEngine &engine, lxb_dom_node_t *node)
	: QObject(&engine), m_engine(engine), m_document(node)
{}

JavascriptHtmlDocument *JavascriptHtmlDocument::fromString(QJSEngine &engine, const QString &html)
{
	auto *document = lxb_html_document_create();
	auto status = lxb_html_document_parse(document, reinterpret_cast<const lxb_char_t *>(html.toStdString().c_str()), html.length());
	if (status != LXB_STATUS_OK) {
		log(QStringLiteral("Error parsing HTML: %1.").arg(status), Logger::Error);
		return nullptr;
	}

	auto *body = lxb_html_document_body_element(document);
	return new JavascriptHtmlDocument(engine, lxb_dom_interface_node(body));
}


QJSValue JavascriptHtmlDocument::outerHTML() const
{
	lexbor_str_t str = {NULL};
	auto status = lxb_html_serialize_tree_str(m_document, &str);
	if (status != LXB_STATUS_OK) {
		log(QStringLiteral("Error serializing HTML node: %1.").arg(status), Logger::Error);
		return QString();
	}
	return QString(reinterpret_cast<const char *>(str.data));
}

QJSValue JavascriptHtmlDocument::innerHTML() const
{
	lexbor_str_t str = {NULL};
	auto status = lxb_html_serialize_deep_str(m_document, &str);
	if (status != LXB_STATUS_OK) {
		log(QStringLiteral("Error serializing HTML node: %1.").arg(status), Logger::Error);
		return QString();
	}
	return QString(reinterpret_cast<const char *>(str.data));
}

QJSValue JavascriptHtmlDocument::innerText() const
{
	lxb_char_t *str = lxb_dom_node_text_content(m_document, NULL);
	return QString(reinterpret_cast<const char *>(str));
}


lxb_status_t find_callback(lxb_dom_node_t *node, lxb_css_selector_specificity_t *spec, void *ctx)
{
	auto *ret = static_cast<QList<lxb_dom_node_t*>*>(ctx);
	ret->append(node);
	return LXB_STATUS_OK;
}

QJSValue JavascriptHtmlDocument::find(const QString &css) const
{
	// Create CSS parser
	auto *parser = lxb_css_parser_create();
	auto parser_status = lxb_css_parser_init(parser, NULL, NULL);
	if (parser_status != LXB_STATUS_OK) {
		log(QStringLiteral("Error creating CSS parser: %1.").arg(parser_status), Logger::Error);
		return QJSValue(QJSValue::UndefinedValue);
	}

	// Create CSS selectors
	auto *selectors = lxb_selectors_create();
	auto selectors_status = lxb_selectors_init(selectors);
	if (selectors_status != LXB_STATUS_OK) {
		log(QStringLiteral("Error creating CSS selectors: %1.").arg(selectors_status), Logger::Error);
		return QJSValue(QJSValue::UndefinedValue);
	}

	// Parse CSS selectors
	auto *list = lxb_css_selectors_parse(parser, reinterpret_cast<const lxb_char_t *>(css.toStdString().c_str()), css.length());
	if (parser->status != LXB_STATUS_OK) {
		log(QStringLiteral("Error parsing CSS selectors: %1.").arg(selectors_status), Logger::Error);
		return QJSValue(QJSValue::UndefinedValue);
	}

	// Find matching HTML nodes
	QList<lxb_dom_node_t*> nodes;
	auto status_find = lxb_selectors_find(selectors, m_document, list, find_callback, &nodes);
	if (status_find != LXB_STATUS_OK) {
		log(QStringLiteral("Error finding nodes via CSS selectors: %1.").arg(status_find), Logger::Error);
		return QJSValue(QJSValue::UndefinedValue);
	}

	// Convert result to QJSValue
	QJSValue js = m_engine.newArray(nodes.length());
	for (int i = 0; i < nodes.length(); ++i) {
		auto *obj = new JavascriptHtmlDocument(m_engine, nodes[i]);
		js.setProperty(i, m_engine.newQObject(obj));
	}
	return js;
}
