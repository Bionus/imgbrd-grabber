#include "source.h"
#include "site.h"
#include "functions.h"



Api::Api(QString name, QMap<QString, QString> data)
	: QObject(), m_name(name), m_data(data)
{
	QString prefix = "Urls/" + m_name;
	for (QString key : m_data.keys())
	{
		if (key.startsWith(prefix))
		{
			QString k = key.right(key.length() - prefix.length() - 1);
			m_data["Urls/" + k] = m_data[key];
		}
	}
}


QString Api::getName() const	{ return m_name;	}
bool Api::needAuth() const	{ return contains("Urls/NeedAuth") && value("Urls/NeedAuth").toLower() == "true";	}

bool Api::contains(QString key) const	{ return m_data.contains(key);	}
QString Api::value(QString key) const	{ return m_data.value(key);		}
