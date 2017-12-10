#include "models/api.h"


Api::Api(const QString &name, const QMap<QString, QString> &data)
	: QObject(), m_name(name), m_data(data)
{
	QString prefix = "Urls/" + m_name;
	for (auto it = m_data.begin(); it != m_data.end(); ++it)
	{
		if (it.key().startsWith(prefix))
		{
			QString k = it.key().right(it.key().length() - prefix.length() - 1);
			m_data["Urls/" + k] = it.value();
		}
	}
}


QString Api::getName() const	{ return m_name;	}
bool Api::needAuth() const	{ return contains("Urls/NeedAuth") && value("Urls/NeedAuth").toLower() == "true";	}

bool Api::contains(const QString &key) const	{ return m_data.contains(key);	}
QString Api::value(const QString &key) const	{ return m_data.value(key);		}
