#include "image-size.h"
#include <QFile>
#include <QFileInfo>


ImageSize::~ImageSize()
{
	if (!m_temporaryPath.isEmpty())
		QFile::remove(m_temporaryPath);
}


QString ImageSize::savePath() const
{
	return m_savePath;
}


QString ImageSize::save(const QString &path)
{
	// If we have a temporary path for this image, we move it to the destination
	if (!m_temporaryPath.isEmpty() && QFile::exists(m_temporaryPath))
	{
		QString temp = m_temporaryPath;
		QFile(m_temporaryPath).rename(path);

		m_temporaryPath.clear();
		m_savePath = path;

		return temp;
	}

	// If we already saved this image somewhere, simply make a copy of this file
	if (!m_savePath.isEmpty() && QFile::exists(m_savePath))
	{
		QFile(m_savePath).copy(path);
		return m_savePath;
	}

	return QString();
}

bool ImageSize::setTemporaryPath(const QString &path)
{
	bool changed = setSavePath(path);

	if (m_temporaryPath == path)
		return changed;

	if (!m_temporaryPath.isEmpty())
		QFile::remove(m_temporaryPath);

	m_temporaryPath = path;

	if (fileSize <= 0)
	{
		fileSize = QFileInfo(m_temporaryPath).size();
		return true;
	}

	return changed;
}

bool ImageSize::setSavePath(const QString &path)
{
	if (path != m_savePath)
	{
		m_savePath = path;

		if (fileSize <= 0)
		{ fileSize = QFileInfo(m_savePath).size(); }

		return true;
	}

	return false;
}


QPixmap ImageSize::pixmap() const
{
	return m_pixmap;
}

const QPixmap &ImageSize::pixmap()
{
	return m_pixmap;
}

void ImageSize::setPixmap(const QPixmap &pixmap)
{
	m_pixmap = !rect.isNull()
		? pixmap.copy(rect)
		: pixmap;
}
