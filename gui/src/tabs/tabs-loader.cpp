#include "tabs-loader.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "ui_tag-tab.h"
#include "ui_pool-tab.h"


bool TabsLoader::load(QString path, QList<tagTab*> &tagTabs, QList<poolTab*> &poolTabs, QList<searchTab*> &allTabs, int &currentTab, Profile *profile, QMap<QString, Site*> &sites, mainWindow *parent)
{
	QFile f(path);
	if (!f.open(QFile::ReadOnly))
	{
		return false;
	}

	// Get the file's header to get the version
	QString header = f.readLine().trimmed();
	f.reset();

	// Version 1 is plain text
	if (!header.startsWith("{"))
	{
		QString links = f.readAll().trimmed();
		f.close();

		QStringList tabs = links.split("\r\n");
		for (int j = 0; j < tabs.size(); j++)
		{
			QStringList infos = tabs[j].split("¤");
			if (infos.size() > 3)
			{
				if (infos[infos.size() - 1] == "pool")
				{
					poolTab *tab = new poolTab(&sites, profile, parent);
					tab->ui->spinPool->setValue(infos[0].toInt());
					tab->ui->comboSites->setCurrentIndex(infos[1].toInt());
					tab->ui->spinPage->setValue(infos[2].toInt());
					tab->ui->spinImagesPerPage->setValue(infos[4].toInt());
					tab->ui->spinColumns->setValue(infos[5].toInt());
					tab->setTags(infos[2]);

					poolTabs.append(tab);
				}
				else
				{
					tagTab *tab = new tagTab(&sites, profile, parent);
					tab->ui->spinPage->setValue(infos[1].toInt());
					tab->ui->spinImagesPerPage->setValue(infos[2].toInt());
					tab->ui->spinColumns->setValue(infos[3].toInt());
					tab->setTags(infos[0]);

					tagTabs.append(tab);
				}
			}
		}
		currentTab = 0;

		return true;
	}

	// Other versions are JSON-based
	else
	{
		QByteArray data = f.readAll();
		QJsonDocument loadDoc = QJsonDocument::fromJson(data);
		QJsonObject object = loadDoc.object();

		int version = object["version"].toInt();
		switch (version)
		{
			case 2:
				currentTab = object["current"].toInt();
				QJsonArray tabs = object["tabs"].toArray();
				for (auto tab : tabs)
				{
					auto infos = tab.toObject();
					QString type = infos["type"].toString();

					if (type == "tag")
					{
						tagTab *tab = new tagTab(&sites, profile, parent);
						if (tab->read(infos))
						{
							tagTabs.append(tab);
							allTabs.append(tab);
						}
					}
					else if (type == "pool")
					{
						poolTab *tab = new poolTab(&sites, profile, parent);
						if (tab->read(infos))
						{
							poolTabs.append(tab);
							allTabs.append(tab);
						}
					}
				}
				return true;
		}

	}

	return false;
}

bool TabsLoader::save(QString path, QList<searchTab*> &allTabs, searchTab *currentTab)
{
	QFile saveFile(path);
	if (!saveFile.open(QFile::WriteOnly))
	{
		return false;
	}

	QJsonArray tabsJson;
	for (auto tab : allTabs)
	{
		QJsonObject tabJson;
		tab->write(tabJson);
		tabsJson.append(tabJson);
	}

	// Generate result
	QJsonObject full;
	full["version"] = 2;
	full["current"] = allTabs.indexOf(currentTab);
	full["tabs"] = tabsJson;

	// Write result
	QJsonDocument saveDoc(full);
	saveFile.write(saveDoc.toJson());
	saveFile.close();

	return true;
}
