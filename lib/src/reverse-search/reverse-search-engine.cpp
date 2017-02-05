#include "reverse-search-engine.h"
#include <QDesktopServices>


ReverseSearchEngine::ReverseSearchEngine(QIcon icon, QString name, QString tpl)
	: m_icon(icon), m_name(name), m_tpl(tpl)
{}

void ReverseSearchEngine::searchByUrl(QUrl url) const
{
	QString tpl = QString(m_tpl);
	tpl.replace("{url}", url.toEncoded());

	QDesktopServices::openUrl(QUrl(tpl));
}


QIcon ReverseSearchEngine::icon() const		{ return m_icon;	}
QString ReverseSearchEngine::name() const	{ return m_name;	}
QString ReverseSearchEngine::tpl() const	{ return m_tpl;		}
