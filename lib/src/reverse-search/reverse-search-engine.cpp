#include "reverse-search-engine.h"
#include "functions.h"
#include <QDesktopServices>


ReverseSearchEngine::ReverseSearchEngine()
{}

ReverseSearchEngine::ReverseSearchEngine(QString icon, QString name, QString tpl)
	: m_icon(loadIcon(icon)), m_name(name), m_tpl(tpl)
{}

QIcon ReverseSearchEngine::loadIcon(QString path) const
{
	if (path.isEmpty())
		return QIcon();

	QFile f(path);
	if (f.open(QFile::ReadOnly))
	{
		QByteArray data12 = f.read(12);
		f.close();

		if (data12.length() >= 12)
		{
			QString ext = getExtensionFromHeader(data12);
			if (!ext.isEmpty())
				return QIcon(QPixmap(path, ext.toStdString().c_str()));
		}
	}

	return QIcon(path);
}

void ReverseSearchEngine::searchByUrl(QUrl url) const
{
	QString tpl = QString(m_tpl);
	tpl.replace("{url}", url.toEncoded());

	QDesktopServices::openUrl(QUrl(tpl));
}


QIcon ReverseSearchEngine::icon() const		{ return m_icon;	}
QString ReverseSearchEngine::name() const	{ return m_name;	}
QString ReverseSearchEngine::tpl() const	{ return m_tpl;		}
