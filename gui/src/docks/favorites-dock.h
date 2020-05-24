#ifndef FAVORITES_DOCK_H
#define FAVORITES_DOCK_H

#include <QList>
#include <QString>
#include <QWidget>
#include "dock.h"


namespace Ui
{
	class FavoritesDock;
}


class Favorite;
class Profile;
class QEvent;
class QPoint;

class FavoritesDock : public Dock
{
	Q_OBJECT

	public:
		explicit FavoritesDock(Profile *profile, QWidget *parent);
		~FavoritesDock() override;

	protected:
		void changeEvent(QEvent *event) override;

	protected slots:
		void changeSortDirection();
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
		Ui::FavoritesDock *ui;
		Profile *m_profile;
		QList<Favorite> &m_favorites;
		bool m_descending = false;
		bool m_isHover = false;
		QString m_hover;
};

#endif // FAVORITES_DOCK_H
