#include "html-node.h"
#include "lexbor/css/css.h"
#include "lexbor/html/html.h"
#include "lexbor/selectors/selectors.h"
#include "logger.h"


HtmlNode::HtmlNode(lxb_dom_node_t *node)
	: m_node(node)
{}

HtmlNode *HtmlNode::fromString(const QString &html, bool fragment)
{
	auto *document = lxb_html_document_create();

	// Parse an HTML fragment
	if (fragment) {
		const QString fragmentWrapper = "p";
		auto *root = lxb_html_document_create_element(document, reinterpret_cast<const lxb_char_t *>(fragmentWrapper.toStdString().c_str()), fragmentWrapper.length(), NULL);
		auto *node = lxb_html_document_parse_fragment(document, &root->element, reinterpret_cast<const lxb_char_t *>(html.toStdString().c_str()), html.length());

		// Ignore the default-built "html" node if there's only one child
		if (node != NULL && node->first_child == node->last_child) {
			node = node->first_child;
		}

		// If no node is found, that means an error occurred
		if (node == NULL) {
			log(QStringLiteral("Error parsing HTML fragment."), Logger::Error);
			return nullptr;
		}

		return new HtmlNode(node);
	}

	// Parse a whole HTML document
	auto status = lxb_html_document_parse(document, reinterpret_cast<const lxb_char_t *>(html.toStdString().c_str()), html.length());
	if (status != LXB_STATUS_OK) {
		log(QStringLiteral("Error parsing HTML: %1.").arg(status), Logger::Error);
		return nullptr;
	}
	const auto *body = lxb_html_document_body_element(document);
	return new HtmlNode(lxb_dom_interface_node(body));
}

HtmlNode::~HtmlNode()
{
	if (m_node->type == LXB_DOM_NODE_TYPE_DOCUMENT) {
		auto *doc = lxb_dom_interface_document(m_node);
		lxb_dom_document_destroy(doc);
	}
}


QString HtmlNode::outerHTML() const
{
	lexbor_str_t str = {NULL};
	auto status = lxb_html_serialize_tree_str(m_node, &str);
	if (status != LXB_STATUS_OK) {
		log(QStringLiteral("Error serializing HTML node: %1.").arg(status), Logger::Error);
		return {};
	}
	return QString(reinterpret_cast<const char *>(str.data));
}

QString HtmlNode::innerHTML() const
{
	lexbor_str_t str = {NULL};
	auto status = lxb_html_serialize_deep_str(m_node, &str);
	if (status != LXB_STATUS_OK) {
		log(QStringLiteral("Error serializing HTML node: %1.").arg(status), Logger::Error);
		return {};
	}
	return QString(reinterpret_cast<const char *>(str.data));
}

QString HtmlNode::innerText() const
{
	lxb_char_t *str = lxb_dom_node_text_content(m_node, NULL);
	return QString(reinterpret_cast<const char *>(str));
}


QString HtmlNode::tag() const
{
	const auto *tagName = lxb_dom_element_qualified_name(lxb_dom_interface_element(m_node), NULL);
	return QString(reinterpret_cast<const char *>(tagName));
}

QString HtmlNode::attr(const QString &attr) const
{
	auto *element = lxb_dom_interface_element(m_node);

	// Check that attribute exists
	bool is_exist = lxb_dom_element_has_attribute(element, reinterpret_cast<const lxb_char_t *>(attr.toStdString().c_str()), attr.length());
	if (!is_exist) {
		return {};
	}

	// Get attribute value from DOM
	const lxb_char_t *value = lxb_dom_element_get_attribute(element, reinterpret_cast<const lxb_char_t *>(attr.toStdString().c_str()), attr.length(), NULL);
	if (value == NULL) {
		log(QStringLiteral("Error getting attribute: %1.").arg(attr), Logger::Error);
		return {};
	}

	return QString(reinterpret_cast<const char *>(value));
}

QStringList HtmlNode::path() const
{
	QStringList ret;

	lxb_dom_node_t *parent = m_node->parent;
	while (parent != NULL) {
		const auto *tagName = lxb_dom_element_qualified_name(lxb_dom_interface_element(parent), NULL);
		ret.prepend(QString(reinterpret_cast<const char *>(tagName)));

		parent = parent->parent;
	}

	return ret;
}

QStringList HtmlNode::pathIds() const
{
	QStringList ret;

	lxb_dom_node_t *parent = m_node->parent;
	while (parent->parent != NULL) {
		ret.prepend(QString::number((quintptr) parent));
		parent = parent->parent;
	}

	return ret;
}


lxb_status_t find_callback(lxb_dom_node_t *node, lxb_css_selector_specificity_t *spec, void *ctx)
{
	Q_UNUSED(spec);
	auto *ret = static_cast<QList<HtmlNode>*>(ctx);
	ret->append(HtmlNode(node));
	return LXB_STATUS_OK;
}

HtmlNode HtmlNode::parent() const
{
	return HtmlNode(m_node->parent);
}

QList<HtmlNode> HtmlNode::find(const QString &css) const
{
	// Create CSS parser
	auto *parser = lxb_css_parser_create();
	auto parser_status = lxb_css_parser_init(parser, NULL, NULL);
	if (parser_status != LXB_STATUS_OK) {
		log(QStringLiteral("Error creating CSS parser: %1.").arg(parser_status), Logger::Error);
		return {};
	}

	// Create CSS selectors
	auto *selectors = lxb_selectors_create();
	auto selectors_status = lxb_selectors_init(selectors);
	if (selectors_status != LXB_STATUS_OK) {
		log(QStringLiteral("Error creating CSS selectors: %1.").arg(selectors_status), Logger::Error);
		return {};
	}

	// Parse CSS selectors
	auto *list = lxb_css_selectors_parse(parser, reinterpret_cast<const lxb_char_t *>(css.toStdString().c_str()), css.length());
	if (parser->status != LXB_STATUS_OK) {
		log(QStringLiteral("Error parsing CSS selectors: %1.").arg(parser->status), Logger::Error);
		return {};
	}

	// Find matching HTML nodes
	QList<HtmlNode> nodes;
	auto status_find = lxb_selectors_find(selectors, m_node, list, find_callback, &nodes);
	if (status_find != LXB_STATUS_OK) {
		log(QStringLiteral("Error finding nodes via CSS selectors: %1.").arg(status_find), Logger::Error);
		return {};
	}

	return nodes;
}
