#ifndef TABS_LOADER_H
#define TABS_LOADER_H

#include <QJsonObject>
#include <QMap>
#include <QString>


class DownloadQueue;
class Profile;
class SearchTab;
class MainWindow;

class TabsLoader
{
	public:
		static bool load(const QString &path, QList<SearchTab*> &allTabs, int &currentTab, Profile *profile, DownloadQueue *downloadQueue, MainWindow *parent);
		static SearchTab *loadTab(QJsonObject info, Profile *profile, DownloadQueue *downloadQueue, MainWindow *parent, bool preload);
		static bool save(const QString &path, QList<SearchTab*> &allTabs, SearchTab *currentTab);
};

#endif // TABS_LOADER_H
