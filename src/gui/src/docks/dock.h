#ifndef DOCK_H
#define DOCK_H

#include <QWidget>


class SearchTab;

class Dock : public QWidget
{
	Q_OBJECT

	public:
		explicit Dock(QWidget *parent);

	public slots:
		virtual void tabChanged(SearchTab *tab);

	protected:
		SearchTab *m_currentTab;
};

#endif // DOCK_H
