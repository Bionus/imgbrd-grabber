#include "secure-file.h"
#include <QCryptographicHash>
#include <QDataStream>


SecureFile::SecureFile(QString filename, QString key)
	: m_file(filename), m_encryptor(generateIntKey(key))
{}

quint64 SecureFile::generateIntKey(const QString &key) const
{
	auto data = QByteArray::fromRawData((const char*)key.utf16(), key.length() * 2);
	QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Md5);
	Q_ASSERT(hash.size() == 16);
	QDataStream stream(hash);
	quint64 a, b;
	stream >> a >> b;
	return a ^ b;
}


void SecureFile::write(QByteArray data)
{
	if (m_file.open(QFile::WriteOnly))
	{
		m_file.write(m_encryptor.encryptToByteArray(data));
		m_file.close();
	}
}

QByteArray SecureFile::readAll()
{
	QByteArray data;
	if (m_file.open(QFile::ReadOnly))
	{
		data = m_encryptor.decryptToByteArray(m_file.readAll());
		m_file.close();
	}
	return data;
}
