#ifndef ADDGROUPWINDOW_H
#define ADDGROUPWINDOW_H

#include <QDialog>
#include <QMap>


namespace Ui
{
	class AddGroupWindow;
}


class Site;
class Profile;
class QSettings;
class TextEdit;
class DownloadQueryGroup;

class AddGroupWindow : public QDialog
{
	Q_OBJECT

	public:
		AddGroupWindow(Site *selected, Profile *profile, QWidget *parent = nullptr);

	public slots:
		void ok();

	signals:
		void sendData(const DownloadQueryGroup &);

	private:
		Ui::AddGroupWindow *ui;
		TextEdit *m_lineTags;
		TextEdit *m_linePostFiltering;
		QMap<QString, Site*> m_sites;
		QSettings *m_settings;
};

#endif // ADDGROUPWINDOW_H
