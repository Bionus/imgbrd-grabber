#ifndef DOWNLOAD_QUERY_GROUP_H
#define DOWNLOAD_QUERY_GROUP_H

#include <QMetaType>
#include <QString>
#include <QSettings>
#include <QJsonObject>
#include "models/site.h"


class DownloadQueryGroup
{
	public:
		// Constructors
		DownloadQueryGroup();
		DownloadQueryGroup(QSettings *settings, QString tags, int page, int perpage, int total, Site *site, QString unk = "");
		DownloadQueryGroup(QString tags, int page, int perpage, int total, bool getBlacklisted, Site *site, QString filename, QString path, QString unk = "");

		// Serialization
		QString toString(QString separator) const;
		void write(QJsonObject &json) const;
		bool read(const QJsonObject &json, QMap<QString, Site*> &sites);

		// Public members
		QString tags;
		int page;
		int perpage;
		int total;
		bool getBlacklisted;
		Site *site;
		QString filename;
		QString path;
		QString unk;
};

Q_DECLARE_METATYPE(DownloadQueryGroup)

#endif // DOWNLOAD_QUERY_GROUP_H
