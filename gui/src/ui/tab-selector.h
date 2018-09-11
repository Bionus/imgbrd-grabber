#ifndef TAB_SELECTOR_H
#define TAB_SELECTOR_H

#include <QList>
#include <QPushButton>


class QAction;
class QMenu;
class QTabWidget;
class QWidget;

class TabSelector : public QPushButton
{
	Q_OBJECT

	public:
		explicit TabSelector(QTabWidget *tabWidget, QWidget *parent = nullptr);

	public slots:
		void setShowTabCount(bool showTabCount);
		void markStaticTab(QWidget *tab);
		void updateCounter();

	private slots:
		void menuAboutToShow();
		void actionTriggered(QAction *action);

	signals:
		void selected(QWidget *widget);

	private:
		QTabWidget *m_tabWidget;
		QMenu *m_menu;
		QList<QWidget*> m_staticTabs;
		bool m_showTabCount = true;
};

#endif // TAB_SELECTOR_H
