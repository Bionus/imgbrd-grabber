#ifndef KEEP_FOR_LATER_DOCK_H
#define KEEP_FOR_LATER_DOCK_H

#include <QString>
#include <QWidget>


namespace Ui
{
	class KeepForLaterDock;
}


class Profile;
class QEvent;

class KeepForLaterDock : public QWidget
{
	Q_OBJECT

	public:
		explicit KeepForLaterDock(Profile *profile, QWidget *parent);
		~KeepForLaterDock() override;

	protected:
		void changeEvent(QEvent *event) override;

	protected slots:
		void refresh();

	signals:
		void open(const QString &tag);
		void openInNewTab(const QString &tag);

	private:
		Ui::KeepForLaterDock *ui;
		Profile *m_profile;
};

#endif // KEEP_FOR_LATER_DOCK_H
