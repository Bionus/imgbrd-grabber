#ifndef IMAGE_SIZE_H
#define IMAGE_SIZE_H

#include <QMetaType>
#include <QPixmap>
#include <QSharedPointer>
#include <QSize>
#include <QString>
#include <QUrl>
#include "loader/downloadable.h"


class QJsonObject;

struct ImageSize
{
	~ImageSize();

	Downloadable::Size type = Downloadable::Size::Unknown;
	QUrl url;
	QSize size;
	qint64 bitRate = 0;
	qint64 fileSize = 0;
	QRect rect;

	// Filesystem cache
	QString save(const QString &path);
	QString savePath() const;
	bool setTemporaryPath(const QString &path);
	bool setSavePath(const QString &path);

	// Pixmap cache
	QPixmap pixmap() const;
	const QPixmap &pixmap();
	void setPixmap(const QPixmap &pixmap);

	// MD5 calculation
	QString md5() const;

	// Serialization
	void read(const QJsonObject &json);
	void write(QJsonObject &json) const;

	private:
		QString m_temporaryPath;
		QString m_savePath;
		QPixmap m_pixmap;
		QString mutable m_md5;
};

Q_DECLARE_METATYPE(ImageSize)
Q_DECLARE_METATYPE(QList<ImageSize>)
Q_DECLARE_METATYPE(QSharedPointer<ImageSize>)
Q_DECLARE_METATYPE(QList<QSharedPointer<ImageSize>>)

#endif // IMAGE_SIZE_H
