#include "image-size.h"
#include <QFile>
#include <QFileInfo>
#include <QJsonObject>


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


void ImageSize::read(const QJsonObject &json)
{
	fileSize = json["fileSize"].toInt();

	if (json.contains("size") && json["size"].isObject()) {
		QJsonObject sz = json["size"].toObject();
		size = QSize(
			sz["width"].toInt(),
			sz["height"].toInt()
		);
	}

	if (json.contains("rect") && json["rect"].isObject()) {
		QJsonObject rct = json["rect"].toObject();
		rect = QRect(
			rct["left"].toInt(),
			rct["top"].toInt(),
			rct["width"].toInt(),
			rct["height"].toInt()
		);
	}
}

void ImageSize::write(QJsonObject &json) const
{
	json["fileSize"] = fileSize;

	if (size.isValid()) {
		QJsonObject sz;
		sz["width"] = size.width();
		sz["height"] = size.height();
		json["size"] = sz;
	}

	if (rect.isValid()) {
		QJsonObject rct;
		rct["left"] = rect.left();
		rct["top"] = rect.top();
		rct["width"] = rect.width();
		rct["height"] = rect.height();
		json["rect"] = rct;
	}
}
