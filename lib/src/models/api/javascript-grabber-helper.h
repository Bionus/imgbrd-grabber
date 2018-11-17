#ifndef JAVASCRIPT_GRABBER_HELPER_H
#define JAVASCRIPT_GRABBER_HELPER_H

#include <QDomNode>
#include <QJSEngine>
#include <QJSValue>
#include <QObject>


class JavascriptGrabberHelper : public QObject
{
	Q_OBJECT

	public:
		explicit JavascriptGrabberHelper(QJSEngine &engine);

		Q_INVOKABLE QJSValue htmlDecode(const QString &txt) const;
		Q_INVOKABLE QJSValue regexMatches(const QString &regex, const QString &txt) const;
		Q_INVOKABLE QJSValue parseXML(const QString &txt) const;

	private:
		QJSValue _parseXMLRec(const QDomNode &node) const;
		QJSEngine &m_engine;
};

#endif // JAVASCRIPT_GRABBER_HELPER_H
