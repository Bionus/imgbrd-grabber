#ifndef TABS_LOADER_H
#define TABS_LOADER_H

#include <QString>
#include <QMap>
#include "models/profile.h"
#include "models/site.h"
#include "tag-tab.h"
#include "pool-tab.h"


class TabsLoader
{
	public:
		static bool load(QString path, QList<tagTab*> &tagTabs, QList<poolTab*> &poolTabs, Profile *profile, QMap<QString, Site *> &sites, mainWindow *parent);
		static bool save(QString path, QList<tagTab*> &tagTabs, QList<poolTab*> &poolTabs, QList<searchTab*> &allTabs);
};

#endif // TABS_LOADER_H
