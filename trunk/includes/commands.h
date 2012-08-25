#ifndef COMMANDS_H
#define COMMANDS_H

#include <QString>
#include <QSettings>
#include "image.h"
#include "tag.h"



struct MysqlSettings
{
	QString driver;
	QString host;
	QString user;
	QString password;
	QString database;
	QString before;
	QString image;
	QString tag;
	QString after;
};

class Commands
{
	public:
		static Commands* get();
		void init(QSettings*);
		bool start();
		bool before();
		bool image(Image*, QString);
		bool tag(Tag);
		bool after();

	private:
		static Commands *_instance;
		QString m_commandImage;
		QString m_commandTag;
		bool m_mysql;
		bool m_started;
		MysqlSettings m_mysqlSettings;
};

#endif // COMMANDS_H
