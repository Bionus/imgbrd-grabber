#include "reverse-search/reverse-search-engine.h"
#include <QDesktopServices>
#include <QUrl>
#include "functions.h"


ReverseSearchEngine::ReverseSearchEngine(int id, const QString &icon, QString name, QString tpl, int order)
	: m_id(id), m_icon(loadIcon(icon)), m_name(std::move(name)), m_tpl(std::move(tpl)), m_order(order)
{}

QIcon ReverseSearchEngine::loadIcon(const QString &path) const
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

void ReverseSearchEngine::searchByUrl(const QUrl &url) const
{
	QString tpl = QString(m_tpl);
	tpl.replace("{url}", url.toEncoded());

	QDesktopServices::openUrl(QUrl(tpl));
}


int ReverseSearchEngine::id() const					{ return m_id;		}
QIcon ReverseSearchEngine::icon() const				{ return m_icon;	}
const QString &ReverseSearchEngine::name() const	{ return m_name;	}
const QString &ReverseSearchEngine::tpl() const		{ return m_tpl;		}
int ReverseSearchEngine::order() const				{ return m_order;	}

void ReverseSearchEngine::setId(int id)			{ m_id = id;		}
void ReverseSearchEngine::setOrder(int order)	{ m_order = order;	}
