#ifndef SOURCE_UPDATER_H
#define SOURCE_UPDATER_H

#include <QObject>
#include <QString>
#include "updater/updater.h"


class SourceUpdater : public Updater
{
	Q_OBJECT

	public:
		SourceUpdater(QString source, QString directory, QString baseUrl);

	public slots:
		void checkForUpdates() const override;

	private slots:
		void checkForUpdatesDone();

	signals:
		void finished(const QString &source, bool isNew);

	private:
		QString m_source;
		QString m_directory;
		QString m_baseUrl;
};

#endif // SOURCE_UPDATER_H
