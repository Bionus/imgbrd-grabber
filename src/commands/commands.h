#ifndef COMMANDS_H
#define COMMANDS_H

#include <QString>
#include <QSettings>
#include "models/image.h"
#include "models/tag.h"
#include "models/profile.h"
#include "sql-worker.h"

struct MysqlSettings
{
	QString before;
	QString tagBefore;
	QString image;
	QString tagAfter;
	QString after;
};


class Commands
{
	public:
		Commands(Profile *profile);
		bool start();
		bool before();
		bool image(const Image &img, QString path);
		bool tag(const Image &img, Tag tag, bool after);
		bool after();
		bool sqlExec(QString sql);

	private:
		Profile *m_profile;

		QString m_commandTagBefore;
		QString m_commandImage;
		QString m_commandTagAfter;

		MysqlSettings m_mysqlSettings;
		SqlWorker *m_sqlWorker;
};

#endif // COMMANDS_H
