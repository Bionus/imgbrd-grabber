#ifndef COMMANDS_H
#define COMMANDS_H

#include <QSettings>
#include <QString>

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
		bool start() const;
		bool before() const;
		bool image(const Image &img, const QString &path);
		bool tag(const Image &img, const Tag &tag, bool after);
		bool after() const;
		bool sqlExec(const QString &sql) const;

	private:
		Profile *m_profile;

		QString m_commandTagBefore;
		QString m_commandImage;
		QString m_commandTagAfter;

		MysqlSettings m_mysqlSettings;
		SqlWorker *m_sqlWorker;
};

#endif // COMMANDS_H
