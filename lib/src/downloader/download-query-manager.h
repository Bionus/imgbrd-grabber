#ifndef DOWNLOAD_QUERY_MANAGER_H
#define DOWNLOAD_QUERY_MANAGER_H

#include <QList>
#include <QObject>
#include <QString>
#include "downloader/download-query-group.h"
#include "downloader/download-query-image.h"


class Profile;

class DownloadQueryManager : public QObject
{
	Q_OBJECT

	public:
		DownloadQueryManager(QString file, Profile *profile);
		void add(const DownloadQueryImage &image);
		void add(const DownloadQueryGroup &group);
		void remove(const DownloadQueryImage &image);
		void remove(const DownloadQueryGroup &group);
		const QList<DownloadQueryImage> &images() const;
		const QList<DownloadQueryGroup> &groups() const;

	public slots:
		bool load();
		bool load(const QString &file);
		bool save() const;
		bool save(const QString &file) const;
		void saveLater() const;

	signals:
		void changed();

	private:
		QString m_file;
		Profile *m_profile;
		QList<DownloadQueryImage> m_images;
		QList<DownloadQueryGroup> m_groups;
		QTimer *m_saveTimer;
};

#endif // DOWNLOAD_QUERY_MANAGER_H
