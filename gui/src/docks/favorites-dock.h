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

	signals:
		void open(const QString &tag);
		void openInNewTab(const QString &tag);

	private:
		Ui::FavoritesDock *ui;
		QList<Favorite> &m_favorites;
		bool m_descending = false;
};

#endif // FAVORITES_DOCK_H
