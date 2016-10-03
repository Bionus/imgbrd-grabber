#include "source.h"
#include "site.h"
#include "functions.h"



Api::Api(QString name, QMap<QString, QString> data)
	: QObject(), m_name(name), m_data(data)
{
	QString sr = m_name;
	if (m_data.contains("Urls/"+sr+"/Tags"))
		m_data["Urls/Tags"] = m_data["Urls/"+sr+"/Tags"];
	if (m_data.contains("Urls/"+sr+"/Home"))
		m_data["Urls/Home"] = m_data["Urls/"+sr+"/Home"];
	if (m_data.contains("Urls/"+sr+"/Pools"))
		m_data["Urls/Pools"] = m_data["Urls/"+sr+"/Pools"];
	if (m_data.contains("Urls/"+sr+"/Login"))
		m_data["Urls/Login"] = m_data["Urls/"+sr+"/Login"];
	if (m_data.contains("Urls/"+sr+"/Limit"))
		m_data["Urls/Limit"] = m_data["Urls/"+sr+"/Limit"];
	if (m_data.contains("Urls/"+sr+"/MaxPage"))
		m_data["Urls/MaxPage"] = m_data["Urls/"+sr+"/MaxPage"];
	if (m_data.contains("Urls/"+sr+"/PagePart"))
		m_data["Urls/PagePart"] = m_data["Urls/"+sr+"/PagePart"];
	if (m_data.contains("Urls/"+sr+"/AltPagePrev"))
		m_data["Urls/AltPagePrev"] = m_data["Urls/"+sr+"/AltPagePrev"];
	if (m_data.contains("Urls/"+sr+"/AltPageNext"))
		m_data["Urls/AltPageNext"] = m_data["Urls/"+sr+"/AltPageNext"];
}


QString Api::getName() const	{ return m_name;	}

bool Api::contains(QString key) const	{ return m_data.contains(key);	}
QString Api::value(QString key) const	{ return m_data.value(key);		}
