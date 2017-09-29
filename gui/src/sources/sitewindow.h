#ifndef SITE_WINDOW_H
#define SITE_WINDOW_H

#include <QDialog>
#include <QMap>


namespace Ui
{
	class SiteWindow;
}


class Site;
class Profile;
class Source;

class SiteWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit SiteWindow(Profile *profile, QMap<QString, Site*> *sites, QWidget *parent = Q_NULLPTR);
		~SiteWindow() override;

	public slots:
		void accept() override;
		void finish(Source *source = nullptr);

	private:
		Ui::SiteWindow *ui;
		Profile *m_profile;
		QList<Source*> *m_sources;
		QMap<QString ,Site*> *m_sites;
		QString m_url;
};

#endif // SITE_WINDOW_H
