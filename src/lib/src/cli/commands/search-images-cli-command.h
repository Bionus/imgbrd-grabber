#ifndef SEARCH_IMAGES_CLI_COMMAND_H
#define SEARCH_IMAGES_CLI_COMMAND_H

#include <QList>
#include <QSharedPointer>
#include <QStringList>
#include "search-cli-command.h"


class Image;
class Profile;
class QObject;
class Site;

class SearchImagesCliCommand : public SearchCliCommand
{
	Q_OBJECT

	public:
		explicit SearchImagesCliCommand(Profile *profile, QStringList tags, QStringList postFiltering, QList<Site*> sites, int page, int perPage, QString filename, QString folder, int max, bool login, bool noDuplicates, bool getBlacklisted, QObject *parent = nullptr);

	protected:
		QList<QSharedPointer<Image>> getAllImages();
		void loadMoreDetails(const QList<QSharedPointer<Image>> &images);

	protected:
		QString m_filename;
		QString m_folder;
		int m_max;
		QString m_blacklist;
		bool m_login;
		bool m_noDuplicates;
		bool m_getBlacklisted;
};

#endif // SEARCH_IMAGES_CLI_COMMAND_H
