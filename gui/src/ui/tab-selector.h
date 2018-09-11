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
		explicit TabSelector(QTabWidget *tabWidget, QPushButton *backButton, QWidget *parent = nullptr);

	public slots:
		void setShowTabCount(bool showTabCount);
		void markStaticTab(QWidget *tab);
		void updateCounter();
		void back();

	private slots:
		void menuAboutToShow();
		void actionTriggered(QAction *action);
		void tabChanged(int index);

	signals:
		void selected(QWidget *widget);

	private:
		QTabWidget *m_tabWidget;
		QPushButton *m_backButton;
		QMenu *m_menu;
		QList<QWidget*> m_staticTabs;
		bool m_showTabCount = true;
		QWidget *m_lastTab = nullptr;
};

#endif // TAB_SELECTOR_H
