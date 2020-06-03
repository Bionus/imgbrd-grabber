#ifndef SECURE_FILE_H
#define SECURE_FILE_H

#include <QByteArray>
#include <QFile>
#include <QString>
#include "vendor/simplecrypt.h"


class SecureFile
{
	public:
		SecureFile(const QString &filename, const QString &key);
		void write(const QByteArray &data);
		QByteArray readAll();

	private:
		quint64 generateIntKey(const QString &key) const;

	private:
		QFile m_file;
		SimpleCrypt m_encryptor;
};

#endif // SECURE_FILE_H
