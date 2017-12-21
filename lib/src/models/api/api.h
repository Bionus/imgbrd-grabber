#ifndef API_H
#define API_H

#include <QMap>
#include <QObject>
#include <QString>
#include "models/image.h"
#include "tags/tag.h"

struct ParsedPage
{
	QString error;
	int imagesCount = -1;
	QList<Tag> tags;
	QList<QSharedPointer<Image>> images;
};

class Api : public QObject
{
	Q_OBJECT

	public:
		Api(const QString &name, const QMap<QString, QString> &data);

		// Getters
		QString getName() const;
		bool needAuth() const;

		// XML info getters
		bool contains(const QString &key) const;
		QString value(const QString &key) const;
		QString operator[](const QString &key) const { return value(key); }

		// API
		virtual ParsedPage parsePage(Page *parentPage, const QString &source, int first) const = 0;

	protected:
		QSharedPointer<Image> parseImage(Page *parentPage, QMap<QString, QString> d, int position, const QList<Tag> &tags = QList<Tag>()) const;

	private:
		QString m_name;
		QMap<QString, QString> m_data;
};

#endif // API_H
