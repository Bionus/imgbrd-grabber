#ifndef ADDUNIQUEWINDOW_H
#define ADDUNIQUEWINDOW_H

#include <QMap>
#include <QDialog>


namespace Ui
{
	class AddUniqueWindow;
}


class Site;
class Profile;
class Image;
class Page;
class DownloadQueryImage;

class AddUniqueWindow : public QDialog
{
	Q_OBJECT

	public:
		AddUniqueWindow(Site *selected, Profile *profile, QWidget *parent = Q_NULLPTR);

	public slots:
		void add();
		void ok(bool close = true);
		void replyFinished(Page *p);
		void addLoadedImage();
		void addImage(QSharedPointer<Image> img);
		void on_buttonFolder_clicked();
		void on_lineFilename_textChanged(QString);

	signals:
		void sendData(const DownloadQueryImage &);

	private:
		Ui::AddUniqueWindow		*ui;
		Page					*m_page;
		QMap<QString,Site*>		m_sites;
		bool					m_close;
		Profile					*m_profile;
		QSharedPointer<Image>	m_image;
};

#endif // ADDUNIQUEWINDOW_H
