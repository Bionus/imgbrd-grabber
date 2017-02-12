#ifndef DOWNLOAD_QUERY_GROUP_H
#define DOWNLOAD_QUERY_GROUP_H

#include <QMetaType>
#include <QString>
#include <QSettings>


class DownloadQueryGroup
{
	public:
		// Constructors
		DownloadQueryGroup();
		DownloadQueryGroup(QSettings *settings, QString tags, int page, int perpage, int total, QString site, QString unk = "");
		DownloadQueryGroup(QString tags, int page, int perpage, int total, bool getBlacklisted, QString site, QString filename, QString path, QString unk = "");

		// Serialization
		QString toString(QString separator) const;

		// Public members
		QString tags;
		int page;
		int perpage;
		int total;
		bool getBlacklisted;
		QString site;
		QString filename;
		QString path;
		QString unk;
};

Q_DECLARE_METATYPE(DownloadQueryGroup)

#endif // DOWNLOAD_QUERY_GROUP_H
