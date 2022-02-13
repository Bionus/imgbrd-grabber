#ifndef HTML_NODE_H
#define HTML_NODE_H

#include <QList>
#include <QString>
#include "lexbor/html/html.h"


class HtmlNode
{
	public:
		HtmlNode(lxb_dom_node_t *node);
		static HtmlNode *fromString(const QString &html);
		~HtmlNode();

		QString outerHTML() const;
		QString innerHTML() const;
		QString innerText() const;

		QString attr(const QString &attr) const;

		QList<HtmlNode> find(const QString &css) const;

	private:
		lxb_dom_node_t *m_node;
};

#endif // HTML_NODE_H
