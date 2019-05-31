#ifndef ADD_UNIQUE_WINDOW_H
#define ADD_UNIQUE_WINDOW_H

#include <QDialog>
#include <QMap>
#include <QQueue>
#include <QSharedPointer>


namespace Ui
{
	class AddUniqueWindow;
}


class Api;
class Site;
class Profile;
class QLabel;
class QPlainTextEdit;
class Image;
class Page;
class DownloadQueryImage;

struct UniqueQuery
{
	Site *site;
	Api *api;
	QString id;
	QString md5;
};

class AddUniqueWindow : public QDialog
{
	Q_OBJECT

	public:
		AddUniqueWindow(Site *selected, Profile *profile, QWidget *parent = nullptr);
		void loadNext();

	public slots:
		void add();
		void ok(bool close = true);
		void replyFinished(Page *p);
		void addLoadedImage();
		void addImage(const QSharedPointer<Image> &img);
		void on_buttonFolder_clicked();
		void on_lineFilename_textChanged(const QString &);
		void toggleMultiLineId(bool toggle);
		void toggleMultiLineMd5(bool toggle);

	signals:
		void sendData(const DownloadQueryImage &);

	protected:
		void next();
		void toggleMultiLine(bool toggle, QPlainTextEdit *plainTextEdit, QLabel *label);

	private:
		Ui::AddUniqueWindow *ui;
		Page *m_page;
		QMap<QString, Site*> m_sites;
		QQueue<UniqueQuery> m_queue;
		bool m_close;
		Profile *m_profile;
		QSharedPointer<Image> m_image;
};

#endif // ADD_UNIQUE_WINDOW_H
