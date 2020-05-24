#ifndef KEEP_FOR_LATER_DOCK_H
#define KEEP_FOR_LATER_DOCK_H

#include <QString>
#include <QWidget>
#include "dock.h"


namespace Ui
{
	class KeepForLaterDock;
}


class Profile;
class QEvent;

class KeepForLaterDock : public Dock
{
	Q_OBJECT

	public:
		explicit KeepForLaterDock(Profile *profile, QWidget *parent);
		~KeepForLaterDock() override;

	protected:
		void changeEvent(QEvent *event) override;

	protected slots:
		void refresh();

		// Context menu
		void setHover(const QString &tag);
		void clearHover();
		void contextMenu(const QPoint &pos);
		void emitOpenInNewTab();

	signals:
		void open(const QString &tag);
		void openInNewTab(const QString &tag);

	private:
		Ui::KeepForLaterDock *ui;
		Profile *m_profile;
		bool m_isHover = false;
		QString m_hover;
};

#endif // KEEP_FOR_LATER_DOCK_H
