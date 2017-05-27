#ifndef REVERSE_SEARCH_ENGINE_H
#define REVERSE_SEARCH_ENGINE_H

#include <QIcon>
#include <QString>
#include <QUrl>


class ReverseSearchEngine
{
	public:
		ReverseSearchEngine();
		ReverseSearchEngine(QIcon icon, QString name, QString tpl);
		void searchByUrl(QUrl url) const;

		QIcon icon() const;
		QString name() const;
		QString tpl() const;

	private:
		QIcon m_icon;
		QString m_name;
		QString m_tpl;
};

#endif // REVERSE_SEARCH_ENGINE_H
