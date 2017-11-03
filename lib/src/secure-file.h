#ifndef SECURE_FILE_H
#define SECURE_FILE_H

#include <QString>
#include <QFile>
#include "vendor/simplecrypt.h"


class SecureFile
{
	public:
		SecureFile(QString filename, QString key);
		void write(QByteArray data);
		QByteArray readAll();

	private:
		quint64 generateIntKey(const QString &key) const;

	private:
		QFile m_file;
		SimpleCrypt m_encryptor;
};

#endif // SECURE_FILE_H
