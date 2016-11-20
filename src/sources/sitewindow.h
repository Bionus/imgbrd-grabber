#ifndef SITE_WINDOW_H
#define SITE_WINDOW_H

#include <QDialog>
#include "models/site.h"



namespace Ui
{
	class SiteWindow;
}



class SiteWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit SiteWindow(QMap<QString,Site*> *sites, QWidget *parent = 0);
		~SiteWindow();

	public slots:
		void accept();
		void finish(Source *source = nullptr);

	private:
		Ui::SiteWindow *ui;
		QList<Source*> *m_sources;
		QMap<QString ,Site*> *m_sites;
		QString m_url;
};

#endif // SITE_WINDOW_H
