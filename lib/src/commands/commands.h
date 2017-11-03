#ifndef COMMANDS_H
#define COMMANDS_H

#include <QString>
#include <QSettings>

struct MysqlSettings
{
	QString before;
	QString tagBefore;
	QString image;
	QString tagAfter;
	QString after;
};


class Image;
class Tag;
class Profile;
class SqlWorker;

class Commands
{
	public:
		explicit Commands(Profile *profile);
		~Commands();
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
