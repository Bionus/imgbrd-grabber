#ifndef ADDGROUPWINDOW_H
#define ADDGROUPWINDOW_H

#include <QDialog>
#include <QStringList>
#include <QSettings>


namespace Ui
{
	class AddGroupWindow;
}


class Site;
class Profile;
class TextEdit;
class DownloadQueryGroup;

class AddGroupWindow : public QDialog
{
	Q_OBJECT

	public:
		AddGroupWindow(QString selected, QMap<QString, Site*> sites, Profile *profile, QWidget *parent);

	public slots:
		void ok();

	signals:
		void sendData(const DownloadQueryGroup &);

	private:
		Ui::AddGroupWindow		*ui;
		TextEdit				*m_lineTags;
		TextEdit				*m_linePostFiltering;
		QMap<QString, Site*>	m_sites;
		QSettings				*m_settings;
};

#endif // ADDGROUPWINDOW_H
