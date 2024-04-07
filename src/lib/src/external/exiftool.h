#ifndef EXIFTOOL_H
#define EXIFTOOL_H

#include <QMap>
#include <QObject>
#include <QProcess>
#include <QString>


class Exiftool : public QObject
{
	Q_OBJECT

	public:
		explicit Exiftool(QObject *parent = nullptr);

		static QString version(int msecs = 30000);

	public slots:
		bool start(int msecs = 30000);
		bool setMetadata(const QString &file, const QMap<QString, QString> &metadata, bool clear = false, int msecs = 30000);
		bool execute(const QString &file, const QString &command, int msecs = 30000);
		bool stop(int msecs = 30000);

	protected slots:
		void onError();

	private:
		QProcess m_process;
};

#endif // EXIFTOOL_H
