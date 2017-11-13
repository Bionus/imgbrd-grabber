#ifndef EXTENSION_ROTATOR_H
#define EXTENSION_ROTATOR_H

#include <QObject>
#include <QString>
#include <QStringList>


class ExtensionRotator : public QObject
{
	Q_OBJECT

	public:
		explicit ExtensionRotator() = default;
		explicit ExtensionRotator(const ExtensionRotator &other);
		explicit ExtensionRotator(QString initialExtension, QStringList extensions, QObject *parent = Q_NULLPTR);
		~ExtensionRotator();
		QString next();

	private:
		QString m_initialExtension;
		QStringList m_extensions;
		int m_next = 0;
};

#endif // EXTENSION_ROTATOR_H
