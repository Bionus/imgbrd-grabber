#ifndef API_H
#define API_H

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>



class Api : public QObject
{
	Q_OBJECT

	public:
		Api(QString name, QMap<QString, QString> data);

		// Getters
		QString getName() const;
		bool needAuth() const;

		// XML info getters
		bool contains(QString key) const;
		QString value(QString key) const;
		QString operator[](QString key) const { return value(key); }

	private:
		QString m_name;
		QMap<QString, QString> m_data;
};

#endif // API_H
