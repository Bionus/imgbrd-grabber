#ifndef LOAD_TAG_DATABASE_CLI_COMMAND_H
#define LOAD_TAG_DATABASE_CLI_COMMAND_H

#include <QList>
#include "cli-command.h"


class Profile;
class QObject;
class Site;
class TagListLoader;

class LoadTagDatabaseCliCommand : public CliCommand
{
	Q_OBJECT

	public:
		explicit LoadTagDatabaseCliCommand(Profile *profile, const QList<Site*> &sites, int minTagCount, QObject *parent = nullptr);

		bool validate() override;
		void run() override;

	protected slots:
		void loadNext();
		void finishedLoading();

	signals:
		void finished(int code);

	private:
		Profile *m_profile;
		QList<Site*> m_sites;
		int m_minTagCount;
};

#endif // LOAD_TAG_DATABASE_CLI_COMMAND_H
