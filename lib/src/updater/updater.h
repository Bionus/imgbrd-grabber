#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <QString>


class NetworkManager;

class Updater : public QObject
{
	Q_OBJECT

	protected:
		Updater();

	public:
		~Updater() override;
		int compareVersions(QString a, QString b);

	public slots:
		virtual void checkForUpdates() const = 0;

	protected:
		NetworkManager *m_networkAccessManager;
};

#endif // UPDATER_H
