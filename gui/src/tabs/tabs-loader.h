#ifndef TABS_LOADER_H
#define TABS_LOADER_H

#include <QString>
#include <QMap>


class Profile;
class Site;
class searchTab;
class tagTab;
class poolTab;
class mainWindow;

class TabsLoader
{
	public:
		static bool load(QString path, QList<searchTab*> &allTabs, int &currentTab, Profile *profile, QMap<QString, Site *> &sites, mainWindow *parent);
		static bool save(QString path, QList<searchTab*> &allTabs, searchTab *currentTab);
};

#endif // TABS_LOADER_H
