#ifndef REVERSE_SEARCH_ENGINE_H
#define REVERSE_SEARCH_ENGINE_H

#include <QIcon>
#include <QString>


class ReverseSearchEngine
{
	public:
		ReverseSearchEngine() = default;
		ReverseSearchEngine(int id, const QString &icon, QString name, QString tpl, int order);
		void searchByUrl(const QUrl &url) const;

		int id() const;
		QIcon icon() const;
		const QString &name() const;
		const QString &tpl() const;
		int order() const;

		void setId(int id);
		void setOrder(int order);

	protected:
		QIcon loadIcon(const QString &path) const;

	private:
		int m_id;
		QIcon m_icon;
		QString m_name;
		QString m_tpl;
		int m_order;
};

#endif // REVERSE_SEARCH_ENGINE_H
