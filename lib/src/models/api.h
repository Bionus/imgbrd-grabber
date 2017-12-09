#ifndef API_H
#define API_H

#include <QMap>
#include <QObject>
#include <QString>


class Api : public QObject
{
	Q_OBJECT

	public:
		Api(QString name, QMap<QString, QString> data);

		// Getters
		QString getName() const;
		bool needAuth() const;

		// XML info getters
		bool contains(const QString &key) const;
		QString value(const QString &key) const;
		QString operator[](const QString &key) const { return value(key); }

	private:
		QString m_name;
		QMap<QString, QString> m_data;
};

#endif // API_H
