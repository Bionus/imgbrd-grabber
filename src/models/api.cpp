#include "source.h"
#include "site.h"
#include "functions.h"



Api::Api(QString name, QMap<QString, QString> data)
	: m_name(name), m_data(data)
{ }


QString Api::getName() const	{ return m_name;	}

bool Api::contains(QString key) const	{ return m_data.contains(key);	}
QString Api::value(QString key) const	{ return m_data.value(key);		}
