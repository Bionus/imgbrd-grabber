#ifndef LOADER_H
#define LOADER_H

#include <QObject>
#include <QString>


class Loader : public QObject
{
	Q_OBJECT

	Q_PROPERTY(Loader::Status status READ status NOTIFY statusChanged)
	Q_PROPERTY(QString error READ error NOTIFY errorChanged)

	public:
		enum Status
		{
			Null,
			Ready,
			Loading,
			Error
		};
		Q_ENUM(Status)

		explicit Loader(QObject *parent = nullptr);

		Loader::Status status() const;
		QString error() const;

	public slots:
		virtual void load() = 0;

	protected:
		void setStatus(Loader::Status status);
		void setError(QString error);

	signals:
		void statusChanged();
		void errorChanged();

	private:
		Loader::Status m_status;
		QString m_error;
};

#endif // LOADER_H
