#ifndef TABS_LOADER_H
#define TABS_LOADER_H

#include <QJsonObject>
#include <QMap>
#include <QString>


class Profile;
class searchTab;
class mainWindow;

class TabsLoader
{
	public:
		static bool load(const QString &path, QList<searchTab*> &allTabs, int &currentTab, Profile *profile, mainWindow *parent);
		static searchTab *loadTab(QJsonObject info, Profile *profile, mainWindow *parent, bool preload);
		static bool save(const QString &path, QList<searchTab*> &allTabs, searchTab *currentTab);
};

#endif // TABS_LOADER_H
