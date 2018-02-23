#ifndef JAVASCRIPT_GRABBER_HELPER_H
#define JAVASCRIPT_GRABBER_HELPER_H

#include <QDomElement>
#include <QJSEngine>
#include <QJSValue>
#include <QObject>


class JavascriptGrabberHelper : public QObject
{
	Q_OBJECT

	public:
		explicit JavascriptGrabberHelper(QJSEngine &engine);

		Q_INVOKABLE QJSValue regexMatches(QString regex, QString txt);
		Q_INVOKABLE QJSValue parseXML(QString txt);

	private:
		QJSValue _parseXMLRec(const QDomNode &node);
		QJSEngine &m_engine;
};

#endif // JAVASCRIPT_GRABBER_HELPER_H
