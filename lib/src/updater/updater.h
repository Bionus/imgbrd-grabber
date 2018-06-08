#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>


class CustomNetworkAccessManager;

class Updater : public QObject
{
	Q_OBJECT

	public:
		Updater();
		~Updater() override;
		int compareVersions(QString a, QString b);

	public slots:
		virtual void checkForUpdates() const = 0;

	protected:
		CustomNetworkAccessManager *m_networkAccessManager;
};

#endif // UPDATER_H
