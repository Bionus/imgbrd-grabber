#ifndef IMAGE_SIZE_H
#define IMAGE_SIZE_H

#include <QPixmap>
#include <QSize>
#include <QString>
#include <QUrl>


struct ImageSize
{
	~ImageSize();

	QSize size;
	int fileSize;

	QString savePath() const;
	bool setTemporaryPath(const QString &path);
	bool setSavePath(const QString &path);

	private:
		QString m_temporaryPath;
		QString m_savePath;
};

#endif // IMAGE_SIZE_H
