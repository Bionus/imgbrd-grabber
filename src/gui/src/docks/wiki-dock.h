#ifndef WIKI_DOCK_H
#define WIKI_DOCK_H

#include <QMetaObject>
#include "dock.h"


namespace Ui
{
	class WikiDock;
}


class QEvent;
class QWidget;
class SearchTab;

class WikiDock : public Dock
{
	Q_OBJECT

	public:
		explicit WikiDock(QWidget *parent);
		~WikiDock() override;

	protected:
		void changeEvent(QEvent *event) override;

	public slots:
		void tabChanged(SearchTab *tab) override;

	protected slots:
		void refresh();

	signals:
		void open(const QString &tag);

	private:
		Ui::WikiDock *ui;
		QMetaObject::Connection m_connection;

};

#endif // WIKI_DOCK_H
