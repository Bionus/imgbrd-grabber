#ifndef TAGS_DOCK_H
#define TAGS_DOCK_H

#include <QMetaObject>
#include "dock.h"


namespace Ui
{
	class TagsDock;
}


class Profile;
class QEvent;
class QWidget;
class SearchTab;

class TagsDock : public Dock
{
	Q_OBJECT

	public:
		explicit TagsDock(Profile *profile, QWidget *parent);
		~TagsDock() override;

	protected:
		void changeEvent(QEvent *event) override;

	public slots:
		void tabChanged(SearchTab *tab) override;

	protected slots:
		void refresh();
		void linkHovered(const QString &tag);
		void contextMenu();
		void emitOpenInNewTab();

	signals:
		void open(const QString &tag);
		void openInNewTab(const QString &tag);

	private:
		Ui::TagsDock *ui;
		Profile *m_profile;
		SearchTab *m_tab;
		QMetaObject::Connection m_connection;
		QString m_link;

};

#endif // TAGS_DOCK_H
