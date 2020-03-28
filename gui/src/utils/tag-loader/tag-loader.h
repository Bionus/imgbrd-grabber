#ifndef TAG_LOADER_H
#define TAG_LOADER_H

#include "models/api/api.h"
#include <QDialog>
#include <QMap>


namespace Ui
{
	class TagLoader;
}


class Profile;
class Site;
class Api;

class TagLoader : public QDialog
{
	Q_OBJECT

	public:
		explicit TagLoader(Profile *profile, QWidget *parent = nullptr);
		~TagLoader() override;

	protected:
		QList<Api*> getApisToLoadTagTypes(Site *site) const;
		QList<Api*> getApisToLoadTags(Site *site, bool needTagTypes) const;

	private slots:
		void start();
		void cancel();

	private:
		Ui::TagLoader *ui;
		Profile *m_profile;
		QMap<QString, Site*> m_sites;
		QStringList m_options;
};

#endif // TAG_LOADER_H
