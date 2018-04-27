#include "models/api/javascript-grabber-helper.h"
#include <QRegularExpression>
#include "logger.h"


JavascriptGrabberHelper::JavascriptGrabberHelper(QJSEngine &engine)
	: m_engine(engine)
{}


QJSValue JavascriptGrabberHelper::regexMatches(QString regex, QString txt)
{
	QJSValue ret = m_engine.newArray();

	QRegularExpression reg(regex, QRegularExpression::DotMatchesEverythingOption);
	const QStringList &groups = reg.namedCaptureGroups();
	auto matches = reg.globalMatch(txt);

	int matchI = 0;
	while (matches.hasNext())
	{
		QJSValue jsMatch = m_engine.newObject();

		auto match = matches.next();
		for (QString group : groups)
		{
			if (group.isEmpty())
				continue;

			QString val = match.captured(group);
			if (val.isEmpty())
				continue;

			int underscorePos = group.lastIndexOf('_');
			bool ok;
			group.midRef(underscorePos + 1).toInt(&ok);
			if (underscorePos != -1 && ok)
			{ group = group.left(underscorePos); }

			jsMatch.setProperty(group, val);
		}

		const QStringList &caps = match.capturedTexts();
		for (int i = 0; i < caps.count(); ++i)
		{ jsMatch.setProperty(i, match.captured(i)); }

		ret.setProperty(matchI++, jsMatch);
	}
	return ret;
}

QJSValue JavascriptGrabberHelper::_parseXMLRec(const QDomNode &node)
{
	QJSValue obj = m_engine.newObject();

	auto type = node.nodeType();

	// Element node
	if (type == QDomNode::ElementNode)
	{
		if (node.attributes().count() > 0)
		{
			QJSValue attributes = m_engine.newObject();
			for (int j = 0; j < node.attributes().count(); j++)
			{
				QDomNode attribute = node.attributes().item(j);
				attributes.setProperty(attribute.nodeName(), attribute.nodeValue());
			}
			obj.setProperty("@attributes", attributes);
		}
	}

	// Text node
	else if (type == QDomNode::TextNode || type == QDomNode::CDATASectionNode)
	{ obj = node.nodeValue(); }

	// Children
	if (node.hasChildNodes())
	{
		for (int i = 0; i < node.childNodes().count(); i++)
		{
			const QDomNode &item = node.childNodes().item(i);
			const QString &nodeName = item.nodeName();
			if (obj.property(nodeName).isUndefined())
			{ obj.setProperty(nodeName, _parseXMLRec(item)); }
			else
			{
				QJSValue prop = obj.property(nodeName);

				if (!obj.property(nodeName).isArray())
				{
					QJSValue old = prop;
					obj.setProperty(nodeName, m_engine.newArray());

					prop = obj.property(nodeName);
					prop.property("push").callWithInstance(prop, QList<QJSValue>() << old);
				}

				prop.property("push").callWithInstance(prop, QList<QJSValue>() << _parseXMLRec(item));
			}
		}
	}

	return obj;
}

QJSValue JavascriptGrabberHelper::parseXML(QString txt)
{
	QDomDocument doc;
	QString errorMsg;
	int errorLine, errorColumn;
	if (!doc.setContent(txt, false, &errorMsg, &errorLine, &errorColumn))
	{
		log(QString("Error parsing XML file: %1 (%2 - %3).").arg(errorMsg, QString::number(errorLine), QString::number(errorColumn)), Logger::Error);
		return QJSValue(QJSValue::UndefinedValue);
	}

	return _parseXMLRec(doc);
}
