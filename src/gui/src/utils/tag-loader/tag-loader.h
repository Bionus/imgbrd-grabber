#ifndef TAG_LOADER_H
#define TAG_LOADER_H

#include "models/api/api.h"
#include <QDialog>
#include <QMap>


namespace Ui
{
	class TagLoader;
}


class Api;
class Profile;
class Site;
class TagListLoader;

class TagLoader : public QDialog
{
	Q_OBJECT

	public:
		explicit TagLoader(Profile *profile, QWidget *parent = nullptr);
		~TagLoader() override;

	private slots:
		void start();
		void cancel();
		void finishedLoading();
		void resetOptions();

	private:
		Ui::TagLoader *ui;
		Profile *m_profile;
		QMap<QString, Site*> m_sites;
		QStringList m_options;
		TagListLoader *m_loader = nullptr;
};

#endif // TAG_LOADER_H
