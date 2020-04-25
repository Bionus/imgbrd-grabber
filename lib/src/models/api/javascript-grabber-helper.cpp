#include "models/api/javascript-grabber-helper.h"
#include <QDomNode>
#include <QJSEngine>
#include <QJSValue>
#include <QRegularExpression>
#include "functions.h"
#include "logger.h"


JavascriptGrabberHelper::JavascriptGrabberHelper(QJSEngine &engine)
	: QObject(&engine), m_engine(engine)
{}


QJSValue JavascriptGrabberHelper::htmlDecode(const QString &txt) const
{
	return decodeHtmlEntities(txt);
}

QJSValue JavascriptGrabberHelper::regexMatches(const QString &regex, const QString &txt) const
{
	QJSValue ret = m_engine.newArray();

	QRegularExpression reg(regex, QRegularExpression::DotMatchesEverythingOption);
	const QStringList &groups = reg.namedCaptureGroups();
	auto matches = reg.globalMatch(txt);

	quint32 matchI = 0;
	while (matches.hasNext()) {
		QJSValue jsMatch = m_engine.newObject();

		auto match = matches.next();
		for (QString group : groups) {
			if (group.isEmpty()) {
				continue;
			}

			QString val = match.captured(group);
			if (val.isEmpty()) {
				continue;
			}

			const int underscorePos = group.lastIndexOf('_');
			bool ok;
			group.midRef(underscorePos + 1).toInt(&ok);
			if (underscorePos != -1 && ok) {
				group = group.left(underscorePos);
			}

			jsMatch.setProperty(group, val);
		}

		const QStringList &caps = match.capturedTexts();
		for (int i = 0; i < caps.count(); ++i) {
			jsMatch.setProperty(i, match.captured(i));
		}

		ret.setProperty(matchI++, jsMatch);
	}

	return ret;
}

QJSValue JavascriptGrabberHelper::_parseXMLRec(const QDomNode &node) const
{
	QJSValue obj = m_engine.newObject();

	const auto type = node.nodeType();

	// Text node
	if (type == QDomNode::TextNode || type == QDomNode::CDATASectionNode) {
		return node.nodeValue();
	}

	// Element node
	if (type == QDomNode::ElementNode) {
		const QDomNamedNodeMap &attributes = node.attributes();
		if (attributes.count() > 0) {
			QJSValue attr = m_engine.newObject();
			for (int j = 0; j < attributes.count(); j++) {
				QDomNode attribute = attributes.item(j);
				attr.setProperty(attribute.nodeName(), attribute.nodeValue());
			}
			obj.setProperty(QStringLiteral("@attributes"), attr);
		}
	}

	// Children
	if (node.hasChildNodes()) {
		const QDomNodeList &children = node.childNodes();
		for (int i = 0; i < children.count(); i++) {
			const QDomNode &item = children.item(i);
			const QString &nodeName = item.nodeName();
			if (obj.property(nodeName).isUndefined()) {
				obj.setProperty(nodeName, _parseXMLRec(item));
			} else {
				QJSValue prop = obj.property(nodeName);

				if (!prop.isArray()) {
					QJSValue newProp = m_engine.newArray();
					newProp.setProperty(0, prop);
					obj.setProperty(nodeName, newProp);
					prop = newProp;
				}

				const quint32 length = prop.property(QStringLiteral("length")).toUInt();
				prop.setProperty(length, _parseXMLRec(item));
			}
		}
	}

	return obj;
}

QJSValue JavascriptGrabberHelper::parseXML(const QString &txt) const
{
	QDomDocument doc;
	QString errorMsg;
	int errorLine, errorColumn;
	if (!doc.setContent(txt, false, &errorMsg, &errorLine, &errorColumn)) {
		log(QStringLiteral("Error parsing XML file: %1 (%2 - %3).").arg(errorMsg, QString::number(errorLine), QString::number(errorColumn)), Logger::Error);
		return QJSValue(QJSValue::UndefinedValue);
	}

	return _parseXMLRec(doc);
}
