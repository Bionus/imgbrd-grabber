#ifndef TAG_LOADER_H
#define TAG_LOADER_H

#include <QDialog>
#include <QMap>


namespace Ui
{
	class TagLoader;
}


class Profile;
class Site;

class TagLoader : public QDialog
{
	Q_OBJECT

	public:
		explicit TagLoader(Profile *profile, QMap<QString, Site*> sites, QWidget *parent = Q_NULLPTR);
		~TagLoader();

	private slots:
		void start();
		void cancel();

	private:
		Ui::TagLoader *ui;
		Profile *m_profile;
		QMap<QString, Site*> m_sites;
};

#endif // TAG_LOADER_H
